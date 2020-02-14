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

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/TmpFile.h"
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

class Signature {
public:
    Signature(const PathName& path, rs_stats_t* stats = nullptr) : signature_(nullptr) {
        AutoStdFile file(path);
        RSCALL(rs_loadsig_file(file, &signature_, stats));
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

Rsync::Rsync() : block_len_(RS_DEFAULT_BLOCK_LEN), strong_len_(0) {}

Rsync::~Rsync() {}

void Rsync::syncData(const PathName& source, const PathName& target) {
    Log::debug<LibEcKit>() << "Rsync::syncData(source=" << source.fullName()
                           << ", target=" << target.fullName() << ")" << std::endl;

    target.touch();
    TmpFile signature;
    Log::debug<LibEcKit>() << "Rsync::syncData using signature file " << signature << std::endl;
    {
        AutoStdFile tgt(target);
        AutoStdFile sig(signature, "w");
        RSCALL(rs_sig_file(tgt, sig, block_len_, strong_len_, RS_RK_BLAKE2_SIG_MAGIC, nullptr));
    }

    TmpFile delta;
    Log::debug<LibEcKit>() << "Rsync::syncData using delta file " << delta << std::endl;
    {
        Signature sig(signature);

        AutoStdFile src(source);
        AutoStdFile dlt(delta, "w");
        RSCALL(rs_delta_file(sig, src, dlt, nullptr));
    }

    PathName patched = PathName::unique(target);
    Log::debug<LibEcKit>() << "Rsync::syncData using temporary output file " << patched << std::endl;
    {
        AutoStdFile tgt(target);
        AutoStdFile dlt(delta);
        AutoStdFile patch(patched, "w");
        RSCALL(rs_patch_file(tgt, dlt, patch, nullptr));
    }
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

    std::vector<PathName> files, dirs;
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
        Log::debug<LibEcKit>() << "Syncing " << file << " -> " << rebased << std::endl;
        syncData(file, rebased);
    }
}

}  // namespace eckit
