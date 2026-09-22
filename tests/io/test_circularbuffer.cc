// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <cmath>
#include <iostream>
#include <string>

#include "eckit/eckit.h"
#include "eckit/io/CircularBuffer.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

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

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
