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

#include "eckit/filesystem/URI.h"
#include "eckit/io/Buffer.h"
#include "eckit/serialisation/ResizableMemoryStream.h"
#include "eckit/testing/Test.h"
#include "eckit/types/Types.h"

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
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "path");
        EXPECT(uri.path() == "local://path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "unix:path");
        EXPECT(uri.asString() == "path");
    }
    {
        URI uri("//:123");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "//:123");
        EXPECT(uri.path() == "local:///:123");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "unix://:123");
        EXPECT(uri.asString() == "//:123");
    }
    {
        URI uri(":path");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "path");
        EXPECT(uri.path() == "local://path");
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
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "path1:path2");
        EXPECT(uri.path() == "local://path1:path2");
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
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "/folder1/folder2/file");
        EXPECT(uri.path() == "local:///folder1/folder2/file");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "unix:/folder1/folder2/file");
        EXPECT(uri.asString() == "/folder1/folder2/file");
    }
    {
        URI uri("foo:");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.authority().empty());
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "foo:");
        EXPECT(uri.path() == "local://foo:");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "unix:foo:");
        EXPECT(uri.asString() == "foo:");
    }
    {
        URI uri("!");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.authority().empty());
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "!");
        EXPECT(uri.path() == "local://!");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "unix:!");
        EXPECT(uri.asString() == "!");
    }
    {
        URI uri("file:!");
        EXPECT(uri.scheme() == "file");
        EXPECT(uri.authority().empty());
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "!");
        EXPECT(uri.path() == "local://!");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "file:!");
        EXPECT(uri.asString() == "!");
    }
    {  // relative path
        URI uri("file:path");
        EXPECT(uri.scheme() == "file");
        EXPECT(uri.authority().empty());
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "path");
        EXPECT(uri.path() == "local://path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "file:path");
        EXPECT(uri.asString() == "path");
    }
    {
        URI uri(":http://nodename/path");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.authority().empty());
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "http://nodename/path");
        EXPECT(uri.path() == "local://http://nodename/path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "unix:http://nodename/path");
        EXPECT(uri.asString() == "http://nodename/path");
    }
}

CASE("Parsing uri (scheme)") {
    {
        URI uri("file:///path");
        EXPECT(uri.scheme() == "file");
        EXPECT(uri.authority().empty());
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "///path");
        EXPECT(uri.path() == "/path");
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
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "/this/is/a/path:with:colons");
        EXPECT(uri.path() == "/this/is/a/path:with:colons");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "unix:/this/is/a/path:with:colons");
        EXPECT(uri.asString() == "/this/is/a/path:with:colons");
    }
}

CASE("Parsing uri (query & fragment)") {
    {
        URI uri("file://host:123/path?query#fragment");
        EXPECT(uri.scheme() == "file");
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "//host:123/path");
        EXPECT(uri.path() == "/host:123/path");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment() == "fragment");
        EXPECT(uri.asRawString() == "file://host:123/path#fragment");
        EXPECT(uri.asString() == "//host:123/path");
    }
    {
        URI uri("file:///path?length=123&foo=bar#fragment");
        EXPECT(uri.scheme() == "file");
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "///path");
        EXPECT(uri.path() == "/path");
        EXPECT(uri.query() == "foo=bar&length=123");
        EXPECT(uri.query("foo") == "bar");
        EXPECT(uri.query("length") == "123");
        EXPECT(uri.query("non existing").empty());
        EXPECT(uri.fragment() == "fragment");
        EXPECT(uri.asRawString() == "file:///path?foo=bar&length=123#fragment");
        EXPECT(uri.asString() == "///path");
    }
    {
        URI uri("file:///path?length=123&foo=bar");
        EXPECT(uri.scheme() == "file");
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "///path");
        EXPECT(uri.path() == "/path");
        EXPECT(uri.query() == "foo=bar&length=123");
        EXPECT(uri.query("foo") == "bar");
        EXPECT(uri.query("length") == "123");
        EXPECT(uri.query("non existing").empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "file:///path?foo=bar&length=123");
        EXPECT(uri.asString() == "///path");
    }
}

CASE("Parsing uri (authority)") {
    {
        URI uri("http://username:password@host:123/path");
        EXPECT(uri.scheme() == "http");
        EXPECT(uri.authority() == "username:password@host:123");
        EXPECT(uri.user() == "username:password");
        EXPECT(uri.host() == "host");
        EXPECT(uri.hostport() == "host:123");
        EXPECT(uri.port() == 123);
        EXPECT(uri.name() == "/path");
        EXPECT_THROWS_AS(uri.path(), NotImplemented);
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "http://username:password@host:123/path");
        EXPECT(uri.asString() == "http://username:password@host:123/path");
    }
    {
        URI uri("http//:123");
        EXPECT(uri.scheme() == "unix");
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.hostport().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "http//:123");
        EXPECT(uri.path() == "local://http//:123");
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "unix:http//:123");
        EXPECT(uri.asString() == "http//:123");
    }
    {
        URI uri("http://username:password@host:123path");
        EXPECT(uri.scheme() == "http");
        EXPECT(uri.authority() == "username:password@host:123");
        EXPECT(uri.user() == "username:password");
        EXPECT(uri.host() == "host");
        EXPECT(uri.hostport() == "host:123");
        EXPECT(uri.port() == 123);
        EXPECT(uri.name() == "path");
        EXPECT_THROWS_AS(uri.path(), NotImplemented);
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "http://username:password@host:123path");
        EXPECT(uri.asString() == "http://username:password@host:123path");
    }
    {
        URI uri("http://host:path");
        EXPECT(uri.scheme() == "http");
        EXPECT(uri.authority() == "host:");
        EXPECT(uri.user().empty());
        EXPECT(uri.host() == "host");
        EXPECT(uri.hostport() == "host:");
        EXPECT(uri.port() == 0);
        EXPECT(uri.name() == "path");
        EXPECT_THROWS_AS(uri.path(), NotImplemented);
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "http://host:path");
        EXPECT(uri.asString() == "http://host:path");
    }
    {
        URI uri("http://www.ecmwf.int");
        EXPECT(uri.scheme() == "http");
        EXPECT(uri.authority() == "www.ecmwf.int");
        EXPECT(uri.user().empty());
        EXPECT(uri.host() == "www.ecmwf.int");
        EXPECT(uri.hostport() == "www.ecmwf.int");
        EXPECT(uri.port() == -1);
        EXPECT(uri.name().empty());
        EXPECT_THROWS_AS(uri.path(), NotImplemented);
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "http://www.ecmwf.int");
        EXPECT(uri.asString() == "http://www.ecmwf.int");
    }
    {
        URI uri("http://www.ecmwf.int:80");
        EXPECT(uri.scheme() == "http");
        EXPECT(uri.authority() == "www.ecmwf.int:80");
        EXPECT(uri.user().empty());
        EXPECT(uri.host() == "www.ecmwf.int");
        EXPECT(uri.hostport() == "www.ecmwf.int:80");
        EXPECT(uri.port() == 80);
        EXPECT(uri.name().empty());
        EXPECT_THROWS_AS(uri.path(), NotImplemented);
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "http://www.ecmwf.int:80");
        EXPECT(uri.asString() == "http://www.ecmwf.int:80");
    }
    {
        URI uri("http://www.ecmwf.int/");
        EXPECT(uri.scheme() == "http");
        EXPECT(uri.authority() == "www.ecmwf.int");
        EXPECT(uri.user().empty());
        EXPECT(uri.host() == "www.ecmwf.int");
        EXPECT(uri.hostport() == "www.ecmwf.int");
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "/");
        EXPECT_THROWS_AS(uri.path(), NotImplemented);
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "http://www.ecmwf.int/");
        EXPECT(uri.asString() == "http://www.ecmwf.int/");
    }
    {
        URI uri("http://host");
        EXPECT(uri.scheme() == "http");
        EXPECT(uri.authority() == "host");
        EXPECT(uri.user().empty());
        EXPECT(uri.host() == "host");
        EXPECT(uri.hostport() == "host");
        EXPECT(uri.port() == -1);
        EXPECT(uri.name().empty());
        EXPECT_THROWS_AS(uri.path(), NotImplemented);
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "http://host");
        EXPECT(uri.asString() == "http://host");
    }
    {
        URI uri("http://nodename/path");
        EXPECT(uri.scheme() == "http");
        EXPECT(uri.authority() == "nodename");
        EXPECT(uri.user().empty());
        EXPECT(uri.host() == "nodename");
        EXPECT(uri.hostport() == "nodename");
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "/path");
        EXPECT_THROWS_AS(uri.path(), NotImplemented);
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "http://nodename/path");
        EXPECT(uri.asString() == "http://nodename/path");
    }
    {
        URI uri("https://localhost:123/path");
        EXPECT(uri.scheme() == "https");
        EXPECT(uri.authority() == "localhost:123");
        EXPECT(uri.user().empty());
        EXPECT(uri.host() == "localhost");
        EXPECT(uri.hostport() == "localhost:123");
        EXPECT(uri.port() == 123);
        EXPECT(uri.name() == "/path");
        EXPECT_THROWS_AS(uri.path(), NotImplemented);
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "https://localhost:123/path");
        EXPECT(uri.asString() == "https://localhost:123/path");
    }
    /*    {
            URI uri("marsfs://nodename/path");
            EXPECT(uri.scheme() == "marsfs");
            EXPECT(uri.authority().empty());
            EXPECT(uri.user().empty());
            EXPECT(uri.host().empty());
            EXPECT(uri.port() == -1);
            EXPECT(uri.name() == "//nodename/path");
            EXPECT(uri.path() == "marsfs://nodename/path");
            EXPECT(uri.query().empty());
            EXPECT(uri.fragment().empty());
            EXPECT(uri.asRawString() == "marsfs://nodename/path");
            EXPECT(uri.asString() == "marsfs://nodename/path");
        }
        {
            URI uri("marsfs://localhost:123/path");
            EXPECT(uri.scheme() == "marsfs");
            EXPECT(uri.authority().empty());
            EXPECT(uri.user().empty());
            EXPECT(uri.host().empty());
            EXPECT(uri.port() == -1);
            EXPECT(uri.name() == "//localhost:123/path");
            EXPECT(uri.path() == "marsfs://localhost:123/path");
            EXPECT(uri.query().empty());
            EXPECT(uri.fragment().empty());
            EXPECT(uri.asRawString() == "marsfs://localhost:123/path");
            EXPECT(uri.asString() == "marsfs://localhost:123/path");
        }*/
}

CASE("Stream") {
    {
        URI uriOrig("http://username:password@host:123/path");

        eckit::Buffer b(1000);  // should be enough
        b.zero();
        eckit::ResizableMemoryStream s(b);

        s << uriOrig;

        s.rewind();

        URI uri(s);
        EXPECT(uri.scheme() == "http");
        EXPECT(uri.authority() == "username:password@host:123");
        EXPECT(uri.user() == "username:password");
        EXPECT(uri.host() == "host");
        EXPECT(uri.port() == 123);
        EXPECT(uri.name() == "/path");
        EXPECT_THROWS_AS(uri.path(), NotImplemented);
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "http://username:password@host:123/path");
        EXPECT(uri.asString() == "http://username:password@host:123/path");
    }
    {
        URI uriOrig("http://username:password@host:123/path");

        eckit::Buffer b(1000);  // should be enough
        b.zero();
        eckit::ResizableMemoryStream s(b);

        s << uriOrig;

        s.rewind();

        URI uri("/");

        s >> uri;
        EXPECT(uri.scheme() == "http");
        EXPECT(uri.authority() == "username:password@host:123");
        EXPECT(uri.user() == "username:password");
        EXPECT(uri.host() == "host");
        EXPECT(uri.port() == 123);
        EXPECT_THROWS_AS(uri.path(), NotImplemented);
        EXPECT(uri.query().empty());
        EXPECT(uri.fragment().empty());
        EXPECT(uri.asRawString() == "http://username:password@host:123/path");
        EXPECT(uri.asString() == "http://username:password@host:123/path");
    }
    {
        URI uriOrig("http:///path?length=123&foo=bar#fragment");
        eckit::Buffer b(1000);  // must be enough
        b.zero();
        eckit::ResizableMemoryStream s(b);

        s << uriOrig;

        s.rewind();

        URI uri(s);
        EXPECT(uri.scheme() == "http");
        EXPECT(uri.user().empty());
        EXPECT(uri.host().empty());
        EXPECT(uri.port() == -1);
        EXPECT(uri.name() == "/path");
        EXPECT_THROWS_AS(uri.path(), NotImplemented);
        EXPECT(uri.query() == "foo=bar&length=123");
        EXPECT(uri.query("foo") == "bar");
        EXPECT(uri.query("length") == "123");
        EXPECT(uri.query("non existing").empty());
        EXPECT(uri.fragment() == "fragment");
        EXPECT(uri.asRawString() == "http:/path?foo=bar&length=123#fragment");
        EXPECT(uri.asString() == "http:/path?foo=bar&length=123#fragment");

        uri.endpoint(eckit::net::Endpoint("host", 123));
        EXPECT(uri.host() == "host");
        EXPECT(uri.port() == 123);
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
