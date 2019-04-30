/*
 * (C) Copyright 1996- ECMWF.
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
#include "eckit/filesystem/PathExpander.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("Expand a CWD") {
    std::string s = "{CWD}/tmp/foo";

    char* e = ::getenv("CURRENT_TEST_DIR");

    EXPECT(e != NULL);

    std::string r = std::string(e) + std::string("/tmp/foo");

    LocalPathName px = PathExpander::expand(s);

    EXPECT(px.realName() == LocalPathName(r).realName());
}

CASE("Expand using missing handler") {
    std::string s = "{FOO}/tmp/foo";

    EXPECT_THROWS_AS(PathExpander::expand(s), eckit::UserError);
}

CASE("Expand an environment variable") {
    std::string s = "{ENVVAR?FOO}/tmp/bar";

    SYSCALL(::setenv("FOO", "/foobar", 1));

    std::string ps = PathExpander::expand(s);
    std::string pr = "/foobar/tmp/bar";

    EXPECT(ps == pr);  // paths dont exist, compare strings
}

static std::string write_file() {  // write file contents
    char* e = ::getenv("CURRENT_TEST_DIR");
    ASSERT(e != NULL);

    PathName foo(e);
    foo /= "foo";

    std::ofstream of(foo.asString().c_str(), std::ofstream::trunc);
    of << "/hoofa/lomp" << std::endl;
    of.close();

    return foo.asString();
}


CASE("Expand with contents of a file") {
    std::string foo = write_file();

    std::string s = "/baz/{FILE?" + foo + "}/tmp/bar";

    LocalPathName px = PathExpander::expand(s);

    std::string r = std::string("/baz/hoofa/lomp/tmp/bar");

    EXPECT(px == LocalPathName(r));  // paths dont exist, compare strings
}

CASE("Expand multiple times, multiple paths") {
    std::string foo = write_file();

    SYSCALL(::setenv("TDIR", "testdir", 1));

    std::string s = "/baz/{ENVVAR?TDIR}/tmp/bar:{FILE?" + foo + "}/xxx";

    LocalPathName px = PathExpander::expand(s);

    std::string r = std::string("/baz/testdir/tmp/bar:/hoofa/lomp/xxx");

    EXPECT(px == LocalPathName(r));  // paths dont exist, compare strings
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
