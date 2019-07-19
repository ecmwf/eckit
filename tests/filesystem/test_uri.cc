/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>

#include "eckit/types/Types.h"

//#include "eckit/filesystem/FileSystemSize.h"
//#include "eckit/filesystem/LocalPathName.h"
//#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/URI.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Parsing uri") {
    URI uri1("path");
    EXPECT(uri1.scheme() == "posix");
    EXPECT(uri1.name() == "path");

    URI uri2(":path");
    EXPECT(uri2.scheme() == "posix");
    EXPECT(uri2.name() == "path");

    URI uri3(":path1:path2");
    EXPECT(uri3.scheme() == "posix");
    EXPECT(uri3.name() == "path1:path2");

    URI uri4(":/folder1/folder2/file");
    EXPECT(uri4.scheme() == "posix");
    EXPECT(uri4.name() == "/folder1/folder2/file");

    URI uri5("scheme://username:password@host:123/path");
    EXPECT(uri5.scheme() == "scheme");
    EXPECT(uri5.host() == "host");
    EXPECT(uri5.port() == "123");
    EXPECT(uri5.path() == "/path");

    URI uri6("scheme://host:123/path?query#fragment");
    EXPECT(uri6.scheme() == "scheme");
    EXPECT(uri6.host() == "host");
    EXPECT(uri6.port() == "123");
    EXPECT(uri6.path() == "/path");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
