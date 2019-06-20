/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cmath>

#include "eckit/config/LibEcKit.h"
#include "eckit/config/Resource.h"
#include "eckit/config/ResourceMgr.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Types.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;
using namespace eckit::types;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_default") {
    std::string s = Resource<std::string>("s", "some");

    EXPECT(s == "some");

    double d = Resource<double>("d", 777.7);

    EXPECT(is_approximately_equal(d, 777.7, 0.0001));  // accept 0.0001% tolerance
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_vector_long") {
    std::vector<long> def(3, 77);
    std::vector<long> v = Resource<std::vector<long> >("listlong;-listlong", def);

    EXPECT(v[0] == 88);
    EXPECT(v[1] == 99);
    EXPECT(v[2] == 11);
    EXPECT(v[3] == 22);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_command_line") {
    int myint = Resource<int>("integer;-integer", 0);
    EXPECT(myint == 100);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_environment_var") {
    char v[] = "TEST_ENV_INT=333";
    putenv(v);

    int intenv = Resource<int>("intEnv;$TEST_ENV_INT", 777);
    EXPECT(intenv == 333);

    char foo[] = "FOO=1Mb";
    putenv(foo);

    long l1 = Resource<long>("$FOO", 0);
    EXPECT(l1 == 1024 * 1024);

    long l2 = Resource<long>("$FOO;-foo", 0);
    EXPECT(l2 == 1024 * 1024);

    long l3 = Resource<long>("-foo;$FOO", 0);
    EXPECT(l3 == 1024 * 1024);

    long l4 = Resource<long>("$FOO;foo;-foo", 0);
    EXPECT(l4 == 1024 * 1024);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("test_libresource") {
    long i = LibResource<long, LibEcKit>("foo-bar;$FOOBAR", 1024 * 1024);

    EXPECT(i == 1024 * 1024);


    char v[] = "TEST_FOOBAZ=fooBAZ";
    putenv(v);
    std::string foobaz = LibResource<std::string, LibEcKit>("foo-baz;$TEST_FOOBAZ", "foobazzzzzzz");

    EXPECT(foobaz == "fooBAZ");
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
