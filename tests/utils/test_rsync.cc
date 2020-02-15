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
#include <string>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/TmpFile.h"
#include "eckit/io/DataHandle.h"
#include "eckit/utils/Rsync.h"

#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::testing;


namespace eckit {
namespace test {

static void removeDir(const PathName& dir) {
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

static bool isSame(const PathName& right, const PathName& left) {
    std::unique_ptr<DataHandle> rhs(right.fileHandle());
    std::unique_ptr<DataHandle> lhs(left.fileHandle());
    return rhs->compare(*lhs);
}

static void isSameDir(const PathName& right, const PathName& left) {
    std::vector<PathName> files;
    std::vector<PathName> dirs;
    right.childrenRecursive(files, dirs);

    for (const auto& dir : dirs) {
        std::cout << "dir: " << dir << std::endl;
    }

    for (const auto& file : files) {
        std::cout << "file: " << file << std::endl;
    }
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
        EXPECT(isSame(source, target));
    }

    SECTION("File sync from empty file ") {
        source.touch();
        fill(target, "The quick brown fox jumps over the lazy dog");

        EXPECT_NO_THROW(rsync.syncData(source, target));
        EXPECT(isSame(source, target));
    }

    SECTION("File sync to existing empty file") {
        fill(source, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor.");
        target.touch();

        EXPECT_NO_THROW(rsync.syncData(source, target));
        EXPECT(isSame(source, target));
    }

    SECTION("Delta in the begining") {
        fill(source,
             "But I must explain to you how all this mistaken idea of denouncing pleasure and praising pain was "
             "born and I will give you a complete account of the system...");

        fill(target,
             "He should complain to you how all this mistaken idea of denouncing pleasure and praising pain was "
             "born and I will give you a complete account of the system...");

        EXPECT_NO_THROW(rsync.syncData(source, target));
        EXPECT(isSame(source, target));
    }

    SECTION("Delta in the middle") {
        fill(source,
             "One morning, when Gregor Samsa woke from troubled dreams, he found himself transformed in his bed "
             "into a horrible vermin.");
        fill(target, "One morning transformed in his bed into a horrible vermin.");

        EXPECT_NO_THROW(rsync.syncData(source, target));
        EXPECT(isSame(source, target));
    }

    SECTION("Delta at the end") {
        fill(source, "The quick brown fox jumps over the lazy dog.");
        fill(target, "The quick brown fox jumps over the curious duck.");

        EXPECT_NO_THROW(rsync.syncData(source, target));
        EXPECT(isSame(source, target));
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

    // SECTION("Sync dirs") {
    //     EXPECT_NO_THROW(rsync.syncRecursive(source, target));
    //     isSameDir(source, target);
    // }

    removeDir(target);
    removeDir(source);
}


}  // end namespace test
}  // end namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
