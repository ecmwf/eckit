/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>

#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/Timer.h"
#include "eckit/utils/xxHash.h"

#include "eckit/test/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;


namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_utils_xxHash_constructor") {
    xxHash hash;

    const char* msg = "The quick brown fox jumps over the lazy dog";

    hash.add(msg, strlen(msg));

    std::string res("0b242d361fda71bc");

    EXPECT(res == hash.digest());
}

CASE("test_eckit_utils_xxHash_constructor_string") {
    xxHash hash("Nobody inspects the spammish repetition");

    std::string res("fbcea83c8a378bf1");

    EXPECT(res == hash.digest());
}

CASE("test_eckit_utils_xxHash_test_suite") {
    EXPECT(xxHash("").digest() == std::string("ef46db3751d8e999"));
    EXPECT(xxHash("a").digest() == std::string("d24ec4f1a98c6e5b"));
    EXPECT(xxHash("abc").digest() == std::string("44bc2cf5ad770999"));
}

CASE("test_eckit_utils_xxhash_compute") {
    std::string msg("The quick brown fox jumps over the lazy dog");

    std::string res("0b242d361fda71bc");

    xxHash hash;
    EXPECT(res == hash.compute(msg.c_str(), msg.size()));
}

CASE("test_eckit_utils_xxhash_reset") {
    xxHash hash("FOOBAR");

    hash.reset();  // reset initial state

    std::string msg("The quick brown fox jumps over the lazy dog");

    hash.add(msg.c_str(), msg.size());

    std::string res("0b242d361fda71bc");

    EXPECT(res == hash.digest());
}

//----------------------------------------------------------------------------------------------------------------------


}  // end namespace test
}  // end namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}