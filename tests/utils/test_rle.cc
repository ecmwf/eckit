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
#include <limits>
#include <memory>
#include <random>
#include <set>
#include <vector>

#include "eckit/config/LibEcKit.h"
#include "eckit/log/Timer.h"
#include "eckit/utils/RLE.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

void test(const std::vector<long>& in, EncodingClock::duration timeout = std::chrono::milliseconds(2), long depth = -1,
          long expectedSize = -1) {

    std::vector<long> out;
    std::vector<long> outTimeout;
    int maxLoop = 5000;

    long long n;
    long long t;
    {
        eckit::Timer timer;
        n = RLEencode2(in.begin(), in.end(), std::back_inserter(out), maxLoop);
        LOG_DEBUG_LIB(LibEcKit) << "no_timeout - time elapsed: " << timer.elapsed() << " - output size: " << out.size()
                                << std::endl;
    }
    {
        eckit::Timer timer;
        t = RLEencode2(in.begin(), in.end(), std::back_inserter(outTimeout), maxLoop, timeout, depth);
        LOG_DEBUG_LIB(LibEcKit) << "timeout " << std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()
                                << "ms - time elapsed: " << timer.elapsed() << " - output size: " << outTimeout.size()
                                << std::endl;
    }

    if (expectedSize > 0) {
        EXPECT_EQUAL(expectedSize, n);
        EXPECT_EQUAL(expectedSize, t);
        EXPECT_EQUAL(out.size(), outTimeout.size());
        for (size_t i = 0; i < out.size(); i++) {
            EXPECT_EQUAL(out[i], outTimeout[i]);
        }
    }

    std::vector<long> decoded;
    RLEdecode2(out.begin(), out.end(), std::back_inserter(decoded));
    EXPECT_EQUAL(in.size(), decoded.size());
    for (size_t i = 0; i < in.size(); i++) {
        EXPECT_EQUAL(in[i], decoded[i]);
    }

    std::vector<long> decodedTimeout;
    RLEdecode2(outTimeout.begin(), outTimeout.end(), std::back_inserter(decodedTimeout));
    EXPECT_EQUAL(in.size(), decodedTimeout.size());
    for (size_t i = 0; i < in.size(); i++) {
        EXPECT_EQUAL(in[i], decodedTimeout[i]);
    }
}

CASE("single") {
    std::vector<long> in = {4};

    test(in, std::chrono::seconds(1), 10, 1);
}

CASE("identical") {
    size_t size = 1000;
    std::vector<long> in;
    in.reserve(size);
    for (size_t i = 0; i < size; i++) {
        in.push_back(4);
    }

    test(in, std::chrono::seconds(1), 10, 2);  /// we expect [-1000,4]
}

CASE("interleaved") {
    size_t size = 1000;
    std::vector<long> in;
    in.reserve(size);
    for (size_t i = 0; i < size; i++) {
        in.push_back(i % 2);
    }

    test(in, std::chrono::seconds(1), 10, 4);  /// we expect [-500,-2,0,1]
}

CASE("pattern") {
    size_t size = 100000;
    std::vector<long> in;
    in.reserve(size);
    for (size_t i = 0; i < size; i++) {
        in.push_back(std::ceil(std::sqrt(i)));
    }

    test(in, std::chrono::milliseconds(50), 1000);
}

void randTest(int limit, int size) {
    std::random_device rd;   // a seed source for the random number engine
    std::mt19937 gen(rd());  // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(1, limit);

    std::vector<long> in;
    in.reserve(size);
    for (size_t i = 0; i < size; i++) {
        in.push_back(distrib(gen));
    }
    test(in, std::chrono::seconds(2), 1000);
}

CASE("randMaxInt") {
    randTest(std::numeric_limits<int>::max(), 10000000);
}

CASE("rand100") {
    randTest(100, 10000000);
}

CASE("rand10") {
    randTest(10, 1000000);
}

CASE("rand2") {
    randTest(2, 1000000);
}
//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
