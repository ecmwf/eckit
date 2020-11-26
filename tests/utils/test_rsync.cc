/*
 * (C) Copyright 2020- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <fstream>
#include <memory>
#include <string>

#include <librsync.h>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/TmpFile.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/MemoryHandle.h"
#include "eckit/io/StdFile.h"
#include "eckit/log/Log.h"
#include "eckit/utils/Rsync.h"

#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::testing;


namespace eckit {
namespace test {

static void remove_dir_recursive(const PathName& dir) {
    if (not dir.exists())
        return;

    ASSERT(dir.isDir());

    std::vector<PathName> files;
    std::vector<PathName> dirs;
    dir.childrenRecursive(files, dirs);

    for (const auto& file : files) {
        file.unlink();
    }

    for (auto d = dirs.rbegin(); d != dirs.rend(); ++d) {
        d->rmdir();
    }

    dir.rmdir();
}

static bool same_contents(const PathName& left, const PathName& right) {
    std::unique_ptr<DataHandle> lhs(left.fileHandle());
    std::unique_ptr<DataHandle> rhs(right.fileHandle());
    return rhs->compare(*lhs);
}

static bool same_dir(const PathName& left, const PathName& right) {
    std::vector<PathName> lfiles;
    std::vector<PathName> ldirs;
    left.childrenRecursive(lfiles, ldirs);

    Log::debug<LibEcKit>() << "left files " << lfiles << std::endl;
    Log::debug<LibEcKit>() << "left dirs " << ldirs << std::endl;

    std::vector<PathName> rfiles;
    std::vector<PathName> rdirs;
    right.childrenRecursive(rfiles, rdirs);

    Log::debug<LibEcKit>() << "right files " << rfiles << std::endl;
    Log::debug<LibEcKit>() << "right dirs " << rdirs << std::endl;

    if (rfiles.size() != lfiles.size() or rdirs.size() != ldirs.size())
        return false;

    // compare files
    {
        std::vector<PathName>::const_iterator riter = rfiles.begin();
        std::vector<PathName>::const_iterator liter = lfiles.begin();

        for (; riter != rfiles.end(); ++riter, ++liter) {
            LocalPathName rhs = LocalPathName(*riter).relativePath(right.localPath());
            LocalPathName lhs = LocalPathName(*liter).relativePath(left.localPath());

            Log::debug<LibEcKit>() << "comparing files " << lhs << " to " << rhs << std::endl;

            if (rhs != lhs)
                return false;

            if (not same_contents(*liter, *riter))
                return false;
        }
    }

    // compare dirs
    {
        std::vector<PathName>::const_iterator riter = rdirs.begin();
        std::vector<PathName>::const_iterator liter = ldirs.begin();

        for (; riter != rdirs.end(); ++riter, ++liter) {
            LocalPathName rhs = LocalPathName(*riter).relativePath(right.localPath());
            LocalPathName lhs = LocalPathName(*liter).relativePath(left.localPath());

            Log::debug<LibEcKit>() << "comparing dirs " << lhs << " to " << rhs << std::endl;

            if (rhs != lhs)
                return false;
        }
    }

    return true;
}

static void fill(const PathName& path, const std::string& msg) {
    std::ofstream ofs(path.localPath());
    ofs << msg << std::endl;
}

CASE("File sync") {

    Rsync rsync;

    PathName source = PathName::unique(PathName(LocalPathName::cwd()) / "test");
    PathName target = PathName::unique(PathName(LocalPathName::cwd()) / "test");

    SECTION("File sync to inexistent target") {
        fill(source, "The quick brown fox jumps over the lazy dog");

        EXPECT_NO_THROW(rsync.syncData(source, target));
        EXPECT(same_contents(source, target));
    }

    SECTION("File sync from empty file ") {
        source.touch();
        fill(target, "The quick brown fox jumps over the lazy dog");

        EXPECT_NO_THROW(rsync.syncData(source, target));
        EXPECT(same_contents(source, target));
    }

    SECTION("File sync to existing empty file") {
        fill(source, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor.");
        target.touch();

        EXPECT_NO_THROW(rsync.syncData(source, target));
        EXPECT(same_contents(source, target));
    }

    SECTION("Delta in the begining") {
        fill(source,
             "But I must explain to you how all this mistaken idea of denouncing pleasure and praising pain was "
             "born and I will give you a complete account of the system...");

        fill(target,
             "He should complain to you how all this mistaken idea of denouncing pleasure and praising pain was "
             "born and I will give you a complete account of the system...");

        EXPECT_NO_THROW(rsync.syncData(source, target));
        EXPECT(same_contents(source, target));
    }

    SECTION("Delta in the middle") {
        fill(source,
             "One morning, when Gregor Samsa woke from troubled dreams, he found himself transformed in his bed "
             "into a horrible vermin.");
        fill(target, "One morning transformed in his bed into a horrible vermin.");

        EXPECT_NO_THROW(rsync.syncData(source, target));
        EXPECT(same_contents(source, target));
    }

    SECTION("Delta at the end") {
        fill(source, "The quick brown fox jumps over the lazy dog.");
        fill(target, "The quick brown fox jumps over the curious duck.");

        EXPECT_NO_THROW(rsync.syncData(source, target));
        EXPECT(same_contents(source, target));
    }

    SECTION("Source file doesnt exist") {
        // no source
        fill(target, "The quick brown fox jumps over the curious duck.");
        EXPECT_THROWS_AS(rsync.syncData(source, target), eckit::CantOpenFile);
    }

    if (source.exists())
        source.unlink();

    if (target.exists())
        target.unlink();
}

CASE("Directory sync") {

    Rsync rsync;

    // fill in source dirctory
    PathName source = PathName::unique(PathName(LocalPathName::cwd()) / "rsync" / "source");
    source.mkdir();

    PathName d1 = source / "dir1";
    d1.mkdir();
    PathName d2 = source / "dir2";
    d2.mkdir();
    PathName d23 = d2 / "dir23";
    d23.mkdir();

    PathName target = PathName::unique(PathName(LocalPathName::cwd()) / "rsync" / "target");

    SECTION("Sync empty dirs") {
        EXPECT_NO_THROW(rsync.syncRecursive(source, target));
        EXPECT(same_dir(source, target));
    }

    SECTION("Sync full dirs") {

        PathName f1d1 = d1 / "f1";
        fill(f1d1, "F1D1");
        PathName f2d1 = d1 / "f2";
        fill(f2d1, "F2D1");
        PathName f3d1 = d1 / "f3";
        fill(f3d1, "F3D1");
        PathName f1d2 = d2 / "f1";
        fill(f1d2, "F1D2");
        PathName f2d2 = d2 / "f2";
        fill(f2d2, "F2D2");
        PathName f1d23 = d23 / "f1";
        fill(f1d23, "F1D23");

        PathName d234 = d23 / "dir234";
        d234.mkdir();
        PathName d12 = d1 / "dir12";
        d12.mkdir();

        EXPECT_NO_THROW(rsync.syncRecursive(source, target));
        EXPECT(same_dir(source, target));
    }

    SECTION("Sync dirs with update") {
        PathName f1d1 = d1 / "f1";
        fill(f1d1, "F1D1bis");
        EXPECT_NO_THROW(rsync.syncRecursive(source, target));
        EXPECT(same_dir(source, target));
    }

    remove_dir_recursive(target);
    remove_dir_recursive(source);
}

CASE("DataHandle operations") {

    Rsync rsync;

    PathName source    = PathName::unique(PathName(LocalPathName::cwd()) / "test");
    PathName target    = PathName::unique(PathName(LocalPathName::cwd()) / "test");
    PathName signature = PathName::unique(PathName(LocalPathName::cwd()) / "test");
    PathName delta     = PathName::unique(PathName(LocalPathName::cwd()) / "test");
    PathName patched   = PathName::unique(PathName(LocalPathName::cwd()) / "test");

    fill(source,
         "But I must explain to you how all this mistaken idea of denouncing pleasure and praising pain was "
         "born and I will give you a complete account of the system...");

    fill(target,
         "He should complain to you how all this mistaken idea of denouncing pleasure and praising pain was "
         "born and I will give you a complete account of the system...");

    {
        AutoStdFile tgt(target);
        AutoStdFile sig(signature, "w");
        ASSERT(rs_sig_file(tgt, sig, RS_DEFAULT_BLOCK_LEN, 0, RS_RK_BLAKE2_SIG_MAGIC, nullptr) == RS_DONE);
    }

    {
        AutoStdFile sigf(signature);
        rs_signature_t* sig;
        ASSERT(rs_loadsig_file(sigf, &sig, nullptr) == RS_DONE);
        ASSERT(rs_build_hash_table(sig) == RS_DONE);

        AutoStdFile src(source);
        AutoStdFile dlt(delta, "w");
        ASSERT(rs_delta_file(sig, src, dlt, nullptr) == RS_DONE);
    }

    {
        AutoStdFile tgt(target);
        AutoStdFile dlt(delta);
        AutoStdFile patch(patched, "w");
        ASSERT(rs_patch_file(tgt, dlt, patch, nullptr) == RS_DONE);
    }

    SECTION("Compute signature") {
        std::unique_ptr<DataHandle> in(target.fileHandle());
        in->openForRead();
        MemoryHandle out;
        out.openForWrite(0);
        EXPECT_NO_THROW(rsync.computeSignature(*in, out));

        std::unique_ptr<DataHandle> ref(signature.fileHandle());
        EXPECT(ref->compare(out));
    }

    SECTION("Compute delta") {
        std::unique_ptr<DataHandle> in(source.fileHandle());
        in->openForRead();
        std::unique_ptr<DataHandle> sig(signature.fileHandle());
        sig->openForRead();
        MemoryHandle out;
        out.openForWrite(0);
        EXPECT_NO_THROW(rsync.computeDelta(*sig, *in, out));

        std::unique_ptr<DataHandle> ref(delta.fileHandle());
        EXPECT(ref->compare(out));
    }

    SECTION("Update data") {
        std::unique_ptr<DataHandle> in(target.fileHandle());
        in->openForRead();
        std::unique_ptr<DataHandle> dlt(delta.fileHandle());
        dlt->openForRead();
        MemoryHandle out;
        out.openForWrite(0);
        EXPECT_NO_THROW(rsync.updateData(*in, *dlt, out));

        std::unique_ptr<DataHandle> ref(patched.fileHandle());
        EXPECT(ref->compare(out));
    }

    if (source.exists())
        source.unlink();

    if (target.exists())
        target.unlink();

    if (signature.exists())
        signature.unlink();

    if (delta.exists())
        delta.unlink();

    if (patched.exists())
        patched.unlink();
}


}  // end namespace test
}  // end namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
