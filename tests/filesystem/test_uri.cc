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
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "path");
        EXPECT(uri.path() == "path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "unix:path");
        EXPECT(uri.asString() == "path");
    }
    {
        URI uri(":path");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "unix:path");
        EXPECT(uri.asString() == "path");
    }
    {
        URI uri(":path1:path2");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "path1:path2");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "unix:path1:path2");
        EXPECT(uri.asString() == "path1:path2");
    }
    {
        URI uri(":/folder1/folder2/file");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.authority().empty());
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "/folder1/folder2/file");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "unix:/folder1/folder2/file");
        EXPECT(uri.asString() == "/folder1/folder2/file");
    }
}

CASE("Parsing uri (scheme)") {
    {
        URI uri("file:///path");
        EXPECT(uri.scheme() == "file");
        EXPECT(uri.authority().empty());
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.path() == "///path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "file:///path");
        EXPECT(uri.asString() == "///path");
    }
    {
        URI uri("/this/is/a/path:with:colons");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.authority().empty());
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.path() == "/this/is/a/path:with:colons");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "unix:/this/is/a/path:with:colons");
        EXPECT(uri.asString() == "/this/is/a/path:with:colons");
    }
}
CASE("Parsing uri (authority)") {
    {
        URI uri("fdb://username:password@host:123/path");
        EXPECT(uri.scheme() == "fdb");
        EXPECT(uri.authority() == "username:password@host:123");
        EXPECT(uri.user() == "username:password");
        EXPECT(uri.host() == "host");
        EXPECT(uri.port() == 123);
        EXPECT(uri.path() == "/path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "fdb://username:password@host:123/path");
        EXPECT(uri.asString() == "fdb://username:password@host:123/path");
    }
    {
        URI uri("fdb://username:password@host:123path");
        EXPECT(uri.scheme() == "fdb");
        EXPECT(uri.authority() == "username:password@host:123");
        EXPECT(uri.user() == "username:password");
        EXPECT(uri.host() == "host");
        EXPECT(uri.port() == 123);
        EXPECT(uri.path() == "path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "fdb://username:password@host:123path");
        EXPECT(uri.asString() == "fdb://username:password@host:123path");
    }
    {
        URI uri("fdb://nodename/path");
        EXPECT(uri.scheme() == "fdb");
        EXPECT(uri.authority() == "nodename");
        EXPECT(uri.user().empty());
        EXPECT(uri.host() == "nodename");
        EXPECT(uri.port() == -1);
        EXPECT(uri.path() == "/path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "fdb://nodename/path");
        EXPECT(uri.asString() == "fdb://nodename/path");
    }
    {
        URI uri("marsfs://nodename/path");
        EXPECT(uri.scheme() == "marsfs");
        EXPECT(uri.authority().empty());
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.path() == "//nodename/path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "marsfs://nodename/path");
        EXPECT(uri.asString() == "marsfs://nodename/path");
    }
    {
        URI uri("fdb://localhost:123/path");
        EXPECT(uri.scheme() == "fdb");
        EXPECT(uri.authority() == "localhost:123");
        EXPECT(uri.user().empty());
        EXPECT(uri.host() == "localhost");
        EXPECT(uri.port() == 123);
        EXPECT(uri.path() == "/path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "fdb://localhost:123/path");
        EXPECT(uri.asString() == "fdb://localhost:123/path");
    }
    {
        URI uri("marsfs://localhost:123/path");
        EXPECT(uri.scheme() == "marsfs");
        EXPECT(uri.authority().empty());
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.path() == "//localhost:123/path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "marsfs://localhost:123/path");
        EXPECT(uri.asString() == "marsfs://localhost:123/path");
    }
}
CASE("Parsing uri (query & fragment)") {
    {
        URI uri("posix://host:123/path?query#fragment");
        EXPECT(uri.scheme() == "posix");
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.path() == "//host:123/path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment() == "fragment");
        EXPECT(uri.asRawString() == "posix://host:123/path#fragment");
        EXPECT(uri.asString() == "//host:123/path#fragment");
    }
    {
        URI uri("posix:///path?length=123&foo=bar#fragment");
        EXPECT(uri.scheme() == "posix");
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.path() == "///path");
        EXPECT(uri.query() == "foo=bar&length=123");
        EXPECT(uri.query("foo") == "bar");
        EXPECT(uri.query("length") == "123");
        EXPECT(uri.query("non existing").empty());
        EXPECT(uri.fragment() == "fragment");
        EXPECT(uri.asRawString() == "posix:///path?foo=bar&length=123#fragment");
        EXPECT(uri.asString() == "///path?foo=bar&length=123#fragment");
    }
    {
        URI uri("posix:///path?length=123&foo=bar");
        EXPECT(uri.scheme() == "posix");
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.path() == "///path");
        EXPECT(uri.query() == "foo=bar&length=123");
        EXPECT(uri.query("foo") == "bar");
        EXPECT(uri.query("length") == "123");
        EXPECT(uri.query("non existing").empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "posix:///path?foo=bar&length=123");
        EXPECT(uri.asString() == "///path?foo=bar&length=123");
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
