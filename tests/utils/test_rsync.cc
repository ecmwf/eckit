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

#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/TmpFile.h"
#include "eckit/utils/Rsync.h"

#include "eckit/testing/Test.h"

using namespace eckit;
using namespace eckit::testing;


namespace eckit {
namespace test {

bool sameContents(PathName file1, PathName file2) {
    std::ifstream ifs1(file1.localPath());
    std::ifstream ifs2(file2.localPath());

    for (std::string line1, line2; !ifs1.eof() && !ifs2.eof(); ) {
        std::getline(ifs1, line1);
        std::getline(ifs2, line2);
        if(line1 != line2)
            return false;
    }

    return ifs1.eof() && ifs2.eof();
}

CASE("File sync") {
    TmpFile source, destination;

    {
        std::ofstream ofs(source.localPath());
        ofs << "The quick brown fox jumps over the lazy dog" << std::endl;
    }
    {
        std::ofstream ofs(destination.localPath());
        ofs << "The quick brown fox jumps over the lazy duck" << std::endl;
    }

    Rsync rsync;
    rsync.syncData(source, destination);

    EXPECT(sameContents(source, destination));
}

}  // end namespace test
}  // end namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
