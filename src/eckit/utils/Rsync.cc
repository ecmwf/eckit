/*
 * (C) Copyright 2020- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <librsync.h>
#include <memory>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/TmpFile.h"
#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/StdFile.h"
#include "eckit/log/Log.h"
#include "eckit/utils/Tokenizer.h"

#include "eckit/utils/Rsync.h"

namespace eckit {

static void handle_rs_error(rs_result res, const char* call, const char* file, int line, const char* func) {
    if (res == RS_DONE)
        return;

    throw new FailedLibraryCall("librsync", call, rs_strerror(res), CodeLocation(file, line, func));
}

#define RSCALL(a) handle_rs_error(a, #a, __FILE__, __LINE__, __func__)


struct handle_with_buffer {
    DataHandle* handle;
    Buffer* buffer;
};

static rs_result fillInputBuffer(rs_job_t* job, rs_buffers_t* buffers, void* opaque) {
    try {
        handle_with_buffer* hwb = reinterpret_cast<handle_with_buffer*>(opaque);
        DataHandle* h           = hwb->handle;
        Buffer& b               = *hwb->buffer;

        char* w    = static_cast<char*>(b);
        size_t len = b.size();
        // *buffers is zeroed before entering this function for the first time
        if (buffers->next_in) {
            ptrdiff_t pos = buffers->next_in - static_cast<char*>(b);
            ASSERT(pos >= 0);
            ASSERT(pos <= b.size());
            ASSERT(pos + buffers->avail_in <= b.size());

            if (pos < b.size()) {
                w   = buffers->next_in + buffers->avail_in;
                len = b.size() - buffers->avail_in - pos;
            }
        }
        else {
            ASSERT(buffers->avail_in == 0);
            buffers->next_in = w;
        }

        if (len == 0)
            return RS_DONE;

        long r_len = h->read(static_cast<void*>(w), len);
        if (r_len <= 0) {
            // XXX: what if there is an error?
            buffers->eof_in = 1;
            return RS_DONE;
        }

        buffers->avail_in += r_len;
    }
    catch (std::exception& e) {
        Log::error() << "eckit::Rsync: exception during read: " << e.what() << std::endl;
        return RS_IO_ERROR;
    }
    catch (...) {
        Log::error() << "eckit::Rsync: unknown exception during read" << std::endl;
        return RS_IO_ERROR;
    }

    return RS_DONE;
}

static rs_result drainOutputBuffer(rs_job_t* job, rs_buffers_t* buffers, void* opaque) {
    try {
        handle_with_buffer* hwb = reinterpret_cast<handle_with_buffer*>(opaque);
        DataHandle* h           = hwb->handle;
        Buffer& b               = *hwb->buffer;

        // first call: initialise output buffer
        if (!buffers->next_out) {
            ASSERT(buffers->avail_out == 0);
            buffers->next_out  = static_cast<char*>(b);
            buffers->avail_out = b.size();
            return RS_DONE;
        }

        ptrdiff_t len = buffers->next_out - static_cast<char*>(b);
        ASSERT(len >= 0);
        ASSERT(len <= b.size());
        ASSERT(len + buffers->avail_out == b.size());

        if (len == 0)
            return RS_DONE;

        long w_len = h->write(b, len);
        if (w_len != len) {
            Log::error() << "wrote only " << w_len << " out of " << len << " to " << *h << std::endl;
            return RS_IO_ERROR;
        }

        buffers->next_out  = static_cast<char*>(b);
        buffers->avail_out = b.size();
    }
    catch (std::exception& e) {
        Log::error() << "eckit::Rsync: exception during write: " << e.what() << std::endl;
        return RS_IO_ERROR;
    }
    catch (...) {
        Log::error() << "eckit::Rsync: unknown exception during write" << std::endl;
        return RS_IO_ERROR;
    }

    return RS_DONE;
}

static void runStreamedJob(rs_job_t* job, DataHandle* input, size_t ibuf_size, DataHandle* output, size_t obuf_size) {

    Buffer ibuf(ibuf_size);
    handle_with_buffer ihwb = {input, &ibuf};

    Buffer obuf(obuf_size);
    handle_with_buffer ohwb = {output, &obuf};

    rs_buffers_t buf;
    RSCALL(rs_job_drive(job, &buf, input ? fillInputBuffer : nullptr, input ? static_cast<void*>(&ihwb) : nullptr,
                        output ? drainOutputBuffer : nullptr, output ? static_cast<void*>(&ohwb) : nullptr));
}


class Signature {
public:
    Signature(const PathName& path, rs_stats_t* stats = nullptr) :
        signature_(nullptr) {
        AutoStdFile file(path);
        RSCALL(rs_loadsig_file(file, &signature_, stats));
        RSCALL(rs_build_hash_table(signature_));
    }

    Signature(DataHandle& input) :
        signature_(nullptr) {
        rs_job_t* job = rs_loadsig_begin(&signature_);
        runStreamedJob(job, &input, 1024 * 16, nullptr, 0);
        RSCALL(rs_build_hash_table(signature_));
    }

    ~Signature() {
        if (signature_)
            rs_free_sumset(signature_);
    }

    operator rs_signature_t*() { return signature_; }

private:
    rs_signature_t* signature_;
};

Rsync::Rsync(bool statistics) :
    block_len_(RS_DEFAULT_BLOCK_LEN), strong_len_(0), statistics_(statistics) {}

Rsync::~Rsync() {}

void logStats(const rs_stats_t* stats, std::ostream& os) {
    char buffer[256];
    rs_format_stats(stats, buffer, sizeof(buffer) - 1);
    os << buffer << std::endl;
}

void Rsync::syncData(const PathName& source, const PathName& target) {
    if (statistics_)
        Log::info() << "Rsync::syncData(source=" << source.fullName() << ", target=" << target.fullName() << ")"
                    << std::endl;
    else
        Log::debug<LibEcKit>() << "Rsync::syncData(source=" << source.fullName() << ", target=" << target.fullName()
                               << ")" << std::endl;

    rs_stats_t stats;

    target.touch();
    TmpFile signature(false);
    Log::debug<LibEcKit>() << "Rsync::syncData using signature file " << signature << std::endl;
    {
        AutoStdFile tgt(target);
        AutoStdFile sig(signature, "w");
        RSCALL(rs_sig_file(tgt, sig, block_len_, strong_len_, RS_RK_BLAKE2_SIG_MAGIC, &stats));
    }
    logStats(&stats, statistics_ ? Log::info() : Log::debug<LibEcKit>());

    TmpFile delta(false);
    Log::debug<LibEcKit>() << "Rsync::syncData using delta file " << delta << std::endl;
    {
        Signature sig(signature);

        AutoStdFile src(source);
        AutoStdFile dlt(delta, "w");
        RSCALL(rs_delta_file(sig, src, dlt, &stats));
    }
    logStats(&stats, statistics_ ? Log::info() : Log::debug<LibEcKit>());

    PathName patched = PathName::unique(target);
    Log::debug<LibEcKit>() << "Rsync::syncData using temporary output file " << patched << std::endl;
    {
        AutoStdFile tgt(target);
        AutoStdFile dlt(delta);
        AutoStdFile patch(patched, "w");
        RSCALL(rs_patch_file(tgt, dlt, patch, &stats));
    }
    logStats(&stats, statistics_ ? Log::info() : Log::debug<LibEcKit>());
    PathName::rename(patched, target);
}

static PathName rebasePath(const PathName& path, const PathName& base, const PathName& newbase) {
    eckit::Tokenizer tokens("/");

    std::vector<std::string> path_tokens;
    tokens(path, path_tokens);

    std::vector<std::string> base_tokens;
    tokens(base, base_tokens);

    std::vector<std::string>::const_iterator pi = path_tokens.begin();
    std::vector<std::string>::const_iterator bi = base_tokens.begin();
    for (; pi != path_tokens.end() && bi != base_tokens.end(); ++pi, ++bi) {
        if (*pi != *bi)
            break;
    }

    ASSERT(bi == base_tokens.end());

    PathName result(newbase);
    for (; pi != path_tokens.end(); ++pi)
        result /= *pi;

    return result;
}

void Rsync::syncRecursive(const PathName& source, const PathName& target) {
    ASSERT(source.isDir());
    target.mkdir();

    std::vector<PathName> files;
    std::vector<PathName> dirs;
    source.childrenRecursive(files, dirs);

    for (const auto& dir : dirs) {
        PathName rebased = rebasePath(dir, source, target);
        Log::debug<LibEcKit>() << "Making sure directory " << rebased << " exists" << std::endl;
        rebased.mkdir();
    }

    for (const auto& file : files) {
        if (file.isLink()) {
            Log::warning() << "eckit::Rsync: skipping " << file << ", which is a symbolic link" << std::endl;
            continue;
        }

        PathName rebased = rebasePath(file, source, target);

        if (!rebased.exists()) {
            Log::debug<LibEcKit>() << "Direct copy " << file << " -> " << rebased << std::endl;
            std::unique_ptr<DataHandle> in(file.fileHandle());
            std::unique_ptr<DataHandle> out(rebased.fileHandle(true));
            in->saveInto(*out);
            continue;
        }

        if (!shouldUpdate(file, rebased)) {
            Log::debug<LibEcKit>() << "eckit::Rsync: skipping " << file << " due to file size / date" << std::endl;
            continue;
        }

        Log::debug<LibEcKit>() << "Syncing " << file << " -> " << rebased << std::endl;
        syncData(file, rebased);
    }
}

bool Rsync::shouldUpdate(const PathName& source, const PathName& target) {
    if (!target.exists())
        return true;

    if (source.size() != target.size())
        return true;

    if (source.lastModified() > target.lastModified())
        return true;

    return false;
}

void Rsync::computeSignature(DataHandle& input, DataHandle& output) {
    rs_job_t* job = rs_sig_begin(block_len_, strong_len_, RS_RK_BLAKE2_SIG_MAGIC);
    runStreamedJob(job, &input, 4 * block_len_, &output, 12 + 4 * (4 + strong_len_));
}

void Rsync::computeDelta(DataHandle& signature, DataHandle& input, DataHandle& output) {
    Signature sig(signature);
    rs_job_t* job = rs_delta_begin(sig);
    runStreamedJob(job, &input, block_len_, &output, 10 + 4 * block_len_);
}


static rs_result readDataHandle(void* opaque, rs_long_t pos, size_t* len, void** buf) {
    try {
        DataHandle* dh = reinterpret_cast<DataHandle*>(opaque);
        dh->seek(pos);

        long rlen = dh->read(*buf, *len);
        if (rlen == 0)
            return RS_INPUT_ENDED;

        *len = rlen;
    }
    catch (std::exception& e) {
        Log::error() << "eckit::Rsync: exception during read: " << e.what() << std::endl;
        return RS_IO_ERROR;
    }
    catch (...) {
        Log::error() << "eckit::Rsync: unknown exception during read" << std::endl;
        return RS_IO_ERROR;
    }

    return RS_DONE;
}

void Rsync::updateData(DataHandle& input, DataHandle& delta, DataHandle& output) {
    rs_job_t* job = rs_patch_begin(readDataHandle, static_cast<void*>(&input));
    runStreamedJob(job, &delta, 64 * 1024, &output, 64 * 1024);
}

}  // namespace eckit
