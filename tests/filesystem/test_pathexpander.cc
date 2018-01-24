/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <string>


#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/LocalPathName.h"
#include "eckit/filesystem/PathExpander.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

CASE("Expand a CWD")
{
    std::string s = "{CWD}/tmp/foo";

    char* e;
    e = ::getenv("CURRENT_TEST_DIR");

    EXPECT(e != NULL);

    std::string r = std::string(e) + std::string("/tmp/foo");

    LocalPathName ps = LocalPathName(PathExpander::expand(s)).realName();
    LocalPathName pr = LocalPathName(r).realName();

    EXPECT( ::strcmp(ps.c_str(), pr.c_str()) == 0 );
}

CASE("Expand using missing handler")
{
    std::string s = "{FOO}/tmp/foo";

    EXPECT_THROWS_AS( PathExpander::expand(s), eckit::UserError );
}

CASE("Expand an environment variable")
{
    std::string s = "{ENVVAR:FOO}/tmp/bar";

    SYSCALL(::setenv("FOO", "/foobar", 1));

    std::string ps = PathExpander::expand(s);
    std::string pr = "/foobar/tmp/bar";

    EXPECT( ps == pr );
}

CASE("Expand multiple times")
{
    std::string s = "{CWD}/baz/{ENVVAR:FOO}/tmp/bar";

    SYSCALL(::setenv("FOO", "foobar", 1));

    std::string r = std::string(::getenv("CURRENT_TEST_DIR"))  + std::string("/baz/foobar/tmp/bar");

    LocalPathName px = PathExpander::expand(s);

    EXPECT( px == r );
}

//-----------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char **argv)
{
    return run_tests ( argc, argv );
}


