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
#include <iostream>
#include <string>

#include "eckit/eckit.h"
#include "eckit/io/CircularBuffer.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_eckit_circularbuffer") {
    CircularBuffer buffer(20);


    for (size_t j = 0; j < 100; ++j) {

        std::string lower;
        for (char c = 'a'; c <= 'z'; ++c) {
            buffer.write(&c, 1);
            lower += c;
        }

        std::string upper;
        for (char c = 'A'; c <= 'Z'; ++c) {
            buffer.write(&c, 1);
            upper += c;
        }

        char q[26];
        EXPECT(buffer.read(q, 26) == 26);
        EXPECT(lower == std::string(q, q + 26));

        EXPECT(buffer.read(q, 26) == 26);
        EXPECT(upper == std::string(q, q + 26));
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
