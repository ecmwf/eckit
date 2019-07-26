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
    {
        URI uri("path");
        EXPECT(uri.scheme() == "posix");
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port().empty());
        EXPECT(uri.name() == "path");
        EXPECT(uri.path() == "path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "posix:path");
        EXPECT(uri.asString() == "path");
    }
    {
        URI uri(":path");
        EXPECT(uri.scheme() == "posix");
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port().empty());
        EXPECT(uri.name() == "path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "posix:path");
        EXPECT(uri.asString() == "path");
    }
    {
        URI uri(":path1:path2");
        EXPECT(uri.scheme() == "posix");
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port().empty());
        EXPECT(uri.name() == "path1:path2");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "posix:path1:path2");
        EXPECT(uri.asString() == "path1:path2");
    }
    {
        URI uri(":/folder1/folder2/file");
        EXPECT(uri.scheme() == "posix");
        EXPECT(uri.authority().empty());
        EXPECT(uri.authority(true).empty());
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port().empty());
        EXPECT(uri.name() == "/folder1/folder2/file");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "posix:/folder1/folder2/file");
        EXPECT(uri.asString() == "/folder1/folder2/file");
    }
    {
        URI uri("file://username:password@host:123/path");
        EXPECT(uri.scheme() == "file");
        EXPECT(uri.authority() == "username:password@host:123");
        EXPECT(uri.authority(true) == "//username:password@host:123");
        EXPECT(uri.user() == "username:password");
        EXPECT(uri.host() == "host");
        EXPECT(uri.port() == "123");
        EXPECT(uri.path() == "/path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "file://username:password@host:123/path");
        EXPECT(uri.asString() == "/path");
    }
    {
        URI uri("unix://host:123/path?query#fragment");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.user().empty());
        EXPECT(uri.host() == "host");
        EXPECT(uri.port() == "123");
        EXPECT(uri.path() == "/path");
        EXPECT(uri.query() == "query");
        EXPECT(uri.fragment() == "fragment");
        EXPECT(uri.query(true) == "?query");
        EXPECT(uri.fragment(true) == "#fragment");
        EXPECT(uri.asRawString() == "unix://host:123/path?query#fragment");
        EXPECT(uri.asString() == "/path");
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
