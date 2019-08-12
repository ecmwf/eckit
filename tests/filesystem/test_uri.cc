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

#include "eckit/filesystem/URI.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Parsing uri (path)") {
    {
        URI uri("path");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.parts()->user().empty());
        EXPECT(uri.parts()->host().empty());
        EXPECT(uri.parts()->port() == -1);
        EXPECT(uri.name() == "path");
        EXPECT(uri.parts()->path() == "path");
        EXPECT(uri.parts()->query().empty());
        EXPECT(uri.parts()->fragment().empty());
        EXPECT(uri.asRawString() == "unix:path");
        EXPECT(uri.asString() == "path");
    }
    {
        URI uri(":path");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.parts()->user().empty());
        EXPECT(uri.parts()->host().empty());
        EXPECT(uri.parts()->port() == -1);
        EXPECT(uri.name() == "path");
        EXPECT(uri.parts()->query().empty());
        EXPECT(uri.parts()->fragment().empty());
        EXPECT(uri.asRawString() == "unix:path");
        EXPECT(uri.asString() == "path");
    }
    {
        URI uri(":path1:path2");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.parts()->user().empty());
        EXPECT(uri.parts()->host().empty());
        EXPECT(uri.parts()->port() == -1);
        EXPECT(uri.name() == "path1:path2");
        EXPECT(uri.parts()->query().empty());
        EXPECT(uri.parts()->fragment().empty());
        EXPECT(uri.asRawString() == "unix:path1:path2");
        EXPECT(uri.asString() == "path1:path2");
    }
    {
        URI uri(":/folder1/folder2/file");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.parts()->authority().empty());
        EXPECT(uri.parts()->user().empty());
        EXPECT(uri.parts()->host().empty());
        EXPECT(uri.parts()->port() == -1);
        EXPECT(uri.name() == "/folder1/folder2/file");
        EXPECT(uri.parts()->query().empty());
        EXPECT(uri.parts()->fragment().empty());
        EXPECT(uri.asRawString() == "unix:/folder1/folder2/file");
        EXPECT(uri.asString() == "/folder1/folder2/file");
    }
}

CASE("Parsing uri (scheme)") {
    {
        URI uri("file:///path");
        EXPECT(uri.scheme() == "file");
        EXPECT(uri.parts()->authority().empty());
        EXPECT(uri.parts()->user().empty());
        EXPECT(uri.parts()->host().empty());
        EXPECT(uri.parts()->port() == -1);
        EXPECT(uri.parts()->path() == "/path");
        EXPECT(uri.parts()->query().empty());
        EXPECT(uri.parts()->fragment().empty());
        EXPECT(uri.asRawString() == "file:///path");
        EXPECT(uri.asString() == "///path");
    }
    {
        URI uri("/this/is/a/path:with:colons");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.parts()->authority().empty());
        EXPECT(uri.parts()->user().empty());
        EXPECT(uri.parts()->host().empty());
        EXPECT(uri.parts()->port() == -1);
        EXPECT(uri.parts()->path() == "/this/is/a/path:with:colons");
        EXPECT(uri.parts()->query().empty());
        EXPECT(uri.parts()->fragment().empty());
        EXPECT(uri.asRawString() == "unix:/this/is/a/path:with:colons");
        EXPECT(uri.asString() == "/this/is/a/path:with:colons");
    }
}
CASE("Parsing uri (authority)") {
    {
        URI uri("file://username:password@host:123/path");
        EXPECT(uri.scheme() == "file");
        EXPECT(uri.parts()->authority() == "username:password@host:123");
        EXPECT(uri.parts()->user() == "username:password");
        EXPECT(uri.parts()->host() == "host");
        EXPECT(uri.parts()->port() == 123);
        EXPECT(uri.parts()->path() == "/path");
        EXPECT(uri.parts()->query().empty());
        EXPECT(uri.parts()->fragment().empty());
        EXPECT(uri.asRawString() == "file://username:password@host:123/path");
        EXPECT(uri.asString() == "//username:password@host:123/path");
    }
    {
        URI uri("file://username:password@host:123path");
        EXPECT(uri.scheme() == "file");
        EXPECT(uri.parts()->authority() == "username:password@host:123");
        EXPECT(uri.parts()->user() == "username:password");
        EXPECT(uri.parts()->host() == "host");
        EXPECT(uri.parts()->port() == 123);
        EXPECT(uri.parts()->path() == "path");
        EXPECT(uri.parts()->query().empty());
        EXPECT(uri.parts()->fragment().empty());
        EXPECT(uri.asRawString() == "file://username:password@host:123path");
        EXPECT(uri.asString() == "//username:password@host:123path");
    }
    {
        URI uri("marsfs://nodename/path");
        EXPECT(uri.scheme() == "marsfs");
        EXPECT(uri.parts()->authority() == "nodename");
        EXPECT(uri.parts()->user().empty());
        EXPECT(uri.parts()->host() == "nodename");
        EXPECT(uri.parts()->port() == -1);
        EXPECT(uri.parts()->path() == "/path");
        EXPECT(uri.parts()->query().empty());
        EXPECT(uri.parts()->fragment().empty());
        EXPECT(uri.asRawString() == "marsfs://nodename/path");
        EXPECT(uri.asString() == "marsfs://nodename/path");
    }
    {
        URI uri("marsfs://localhost:123/path");
        EXPECT(uri.scheme() == "marsfs");
        EXPECT(uri.parts()->authority() == "localhost:123");
        EXPECT(uri.parts()->user().empty());
        EXPECT(uri.parts()->host() == "localhost");
        EXPECT(uri.parts()->port() == 123);
        EXPECT(uri.parts()->path() == "/path");
        EXPECT(uri.parts()->query().empty());
        EXPECT(uri.parts()->fragment().empty());
        EXPECT(uri.asRawString() == "marsfs://localhost:123/path");
        EXPECT(uri.asString() == "marsfs://localhost:123/path");
    }
}
CASE("Parsing uri (query & fragment)") {
    {
        URI uri("posix://host:123/path?query#fragment");
        EXPECT(uri.scheme() == "posix");
        EXPECT(uri.parts()->user().empty());
        EXPECT(uri.parts()->host() == "host");
        EXPECT(uri.parts()->port() == 123);
        EXPECT(uri.parts()->path() == "/path");
        EXPECT(uri.parts()->query().empty());
        EXPECT(uri.parts()->fragment() == "fragment");
        EXPECT(uri.asRawString() == "posix://host:123/path?query#fragment");
        EXPECT(uri.asString() == "//host:123/path?query#fragment");
    }
    {
        URI uri("posix:///path?length=123&foo=bar#fragment");
        EXPECT(uri.scheme() == "posix");
        EXPECT(uri.parts()->user().empty());
        EXPECT(uri.parts()->host().empty());
        EXPECT(uri.parts()->port() == -1);
        EXPECT(uri.parts()->path() == "/path");
        EXPECT(uri.parts()->query() == "foo=bar&length=123");
        EXPECT(uri.parts()->query("foo") == "bar");
        EXPECT(uri.parts()->query("length") == "123");
        EXPECT(uri.parts()->query("non existing").empty());
        EXPECT(uri.parts()->fragment() == "fragment");
        EXPECT(uri.asRawString() == "posix:///path?length=123&foo=bar#fragment");
        EXPECT(uri.asString() == "///path?length=123&foo=bar#fragment");
    }
    {
        URI uri("posix:///path?length=123&foo=bar");
        EXPECT(uri.scheme() == "posix");
        EXPECT(uri.parts()->user().empty());
        EXPECT(uri.parts()->host().empty());
        EXPECT(uri.parts()->port() == -1);
        EXPECT(uri.parts()->path() == "/path");
        EXPECT(uri.parts()->query() == "foo=bar&length=123");
        EXPECT(uri.parts()->query("foo") == "bar");
        EXPECT(uri.parts()->query("length") == "123");
        EXPECT(uri.parts()->query("non existing").empty());
        EXPECT(uri.parts()->fragment().empty());
        EXPECT(uri.asRawString() == "posix:///path?length=123&foo=bar");
        EXPECT(uri.asString() == "///path?length=123&foo=bar");
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
