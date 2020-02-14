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
#include "eckit/io/DataHandle.h"
#include "eckit/io/StdFile.h"
#include "eckit/log/Log.h"
#include "eckit/utils/Tokenizer.h"

#include "eckit/utils/Rsync.h"

namespace eckit {

static void handle_rs_error(rs_result res, const char *call, const char *file, int line, const char *func) {
    if(res == RS_DONE)
        return;

    throw new FailedLibraryCall("librsync", call, rs_strerror(res), CodeLocation(file, line, func));
}

#define RSCALL(a) handle_rs_error(a, #a, __FILE__, __LINE__, __func__)

class Signature {
public:

    Signature(const PathName& path, rs_stats_t *stats = nullptr) : sig_(nullptr) {
        AutoStdFile sig_file(path);
        RSCALL(rs_loadsig_file(sig_file, &sig_, stats));
        RSCALL(rs_build_hash_table(sig_));
    }

    ~Signature() {
        if (sig_)
            rs_free_sumset(sig_);
    }

    operator rs_signature_t*() {
        return sig_;
    }

private:

    rs_signature_t *sig_;
};

Rsync::Rsync() : block_len_(RS_DEFAULT_BLOCK_LEN), strong_len_(0) {
}

Rsync::~Rsync() {
}

void Rsync::syncData(const PathName& source, const PathName& destination) {
    Log::debug<LibEcKit>() << "Rsync::syncFile(source=" << source.fullName()
        << ", destination=" << destination.fullName() << ")" << std::endl;

    destination.touch();
    TmpFile signature;
    Log::debug<LibEcKit>() << "Rsync::syncFile using signature file " << signature << std::endl;
    {
        AutoStdFile destinationFile(destination);
        AutoStdFile signatureFile(signature, "w");
        RSCALL(rs_sig_file(destinationFile, signatureFile, block_len_, strong_len_,
            RS_RK_BLAKE2_SIG_MAGIC, nullptr));
    }

    TmpFile delta;
    Log::debug<LibEcKit>() << "Rsync::syncFile using delta file " << delta << std::endl;
    {
        Signature sig(signature);

        AutoStdFile sourceFile(source);
        AutoStdFile deltaFile(delta, "w");
        RSCALL(rs_delta_file(sig, sourceFile, deltaFile, nullptr));
    }

    TmpFile patched;
    Log::debug<LibEcKit>() << "Rsync::syncFile using temporary output file " << patched << std::endl;
    {
        AutoStdFile destinationFile(destination);
        AutoStdFile deltaFile(delta);
        AutoStdFile patchedFile(patched, "w");
        RSCALL(rs_patch_file(destinationFile, deltaFile, patchedFile, nullptr));
    }

    std::unique_ptr<DataHandle> tmp_handle(patched.fileHandle());
    tmp_handle->saveInto(destination);
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

void Rsync::syncRecursive(const PathName& source_path, const PathName& destination_path) {
    ASSERT(source_path.isDir());
    destination_path.mkdir();

    std::vector<PathName> files, dirs;
    source_path.childrenRecursive(files, dirs);

    for (const auto& dir : dirs) {
        PathName dstdir = rebasePath(dir, source_path, destination_path);
        Log::debug<LibEcKit>() << "Making sure directory " << dstdir << " exists" << std::endl;
        dstdir.mkdir();
    }

    for (const auto& file : files) {
        if (file.isLink()) {
            Log::warning() << "eckit::Rsync: skipping " << file << ", which is a symbolic link" << std::endl;
            continue;
        }

        PathName dstfile = rebasePath(file, source_path, destination_path);
        Log::debug<LibEcKit>() << "Syncing " << file << " -> " << dstfile << std::endl;
        syncData(file, dstfile);
    }
}

}

