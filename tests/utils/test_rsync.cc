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

bool isSame(PathName right, PathName left) {
    std::unique_ptr<DataHandle> rhs(right.fileHandle());
    std::unique_ptr<DataHandle> lhs(left.fileHandle());
    return rhs->compare(*lhs);
}

CASE("File sync to inexistent file") {

    PathName source = PathName::unique(LocalPathName::cwd());
    {
        std::ofstream ofs(source.localPath());
        ofs << "The quick brown fox jumps over the lazy dog" << std::endl;
    }

    PathName target = PathName::unique(LocalPathName::cwd());

    Rsync rsync;
    rsync.syncData(source, target);
    EXPECT(isSame(source, target));
}

CASE("File sync from empty file ") {

    PathName source = PathName::unique(LocalPathName::cwd());
    source.touch();

    PathName target = PathName::unique(LocalPathName::cwd());
    {
        std::ofstream ofs(source.localPath());
        ofs << "The quick brown fox jumps over the lazy dog" << std::endl;
    }

    Rsync rsync;
    rsync.syncData(source, target);
    EXPECT(isSame(source, target));
}

CASE("File sync to existing empty file") {

    PathName source = PathName::unique(LocalPathName::cwd());
    {
        std::ofstream ofs(source.localPath());
        ofs << "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor."
            << std::endl;
    }

    PathName target = PathName::unique(LocalPathName::cwd());
    target.touch();

    Rsync rsync;
    rsync.syncData(source, target);
    EXPECT(isSame(source, target));
}

CASE("Delta in the begining") {

    PathName source = PathName::unique(LocalPathName::cwd());
    {
        std::ofstream ofs(source.localPath());
        ofs << "But I must explain to you how all this mistaken idea of denouncing pleasure and praising pain was born "
               "and I will give you a complete account of the system..."
            << std::endl;
    }

    PathName target = PathName::unique(LocalPathName::cwd());
    {
        std::ofstream ofs(target.localPath());
        ofs << "He should explain to you how all this mistaken idea of denouncing pleasure and praising pain was born "
               "and I will give you a complete account of the system..."
            << std::endl;
    }

    Rsync rsync;
    rsync.syncData(source, target);
    EXPECT(isSame(source, target));
}

CASE("Delta in the middle") {

    PathName source = PathName::unique(LocalPathName::cwd());
    {
        std::ofstream ofs(source.localPath());
        ofs << "One morning, when Gregor Samsa woke from troubled dreams, he found himself transformed in his bed into "
               "a horrible vermin."
            << std::endl;
    }

    PathName target = PathName::unique(LocalPathName::cwd());
    {
        std::ofstream ofs(target.localPath());
        ofs << "One morning transformed in his bed into a horrible vermin." << std::endl;
    }

    Rsync rsync;
    rsync.syncData(source, target);
    EXPECT(isSame(source, target));
}

CASE("Delta at the end") {

    PathName source = PathName::unique(LocalPathName::cwd());
    {
        std::ofstream ofs(source.localPath());
        ofs << "The quick brown fox jumps over the lazy dog" << std::endl;
    }

    PathName target = PathName::unique(LocalPathName::cwd());
    {
        std::ofstream ofs(target.localPath());
        ofs << "The quick brown fox jumps over the curious duck" << std::endl;
    }

    Rsync rsync;
    rsync.syncData(source, target);
    EXPECT(isSame(source, target));
}

}  // end namespace test
}  // end namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
