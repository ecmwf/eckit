// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <cassert>
#include <iostream>
#include <memory>

#include "eckit/io/Buffer.h"
#include "eckit/log/Bytes.h"
#include "eckit/log/Seconds.h"
#include "eckit/log/Timer.h"
#include "eckit/testing/Test.h"
#include "eckit/utils/Hash.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

template <int N, int M>
void timeAdd(Hash& hash, eckit::Buffer& buffer, eckit::Timer& timer) {

    timer.start();

    for (int i = 0; i < N; ++i) {
        hash.reset();
        for (size_t j = 0; j < M; ++j) {
            hash.add(buffer, buffer.size());
        }
        hash.digest();
    }

    timer.stop();

    std::cout << " - " << N << " x " << M << " x add(" << Bytes(buffer.size()) << ") rate "
              << Bytes(N * M * buffer.size(), timer) << std::endl;
}

template <int N>
void timeCompute(Hash& hash, eckit::Buffer& buffer, eckit::Timer& timer) {

    timer.start();

    for (int i = 0; i < N; ++i) {
        std::string s = hash.compute(buffer, buffer.size());
        assert(!s.empty());
    }

    timer.stop();

    std::cout << " - " << N << " x compute(" << Bytes(buffer.size()) << ") rate " << Bytes(N * buffer.size(), timer)
              << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

CASE("Test hash performance") {

    eckit::Buffer buffer(4 * 1024 * 1024);
    eckit::Buffer buffer2(64 * 1024 * 1024);
    eckit::Timer timer;

    std::vector<std::string> hashes{"xxh64", "MD4", "MD5", "SHA1"};
    for (auto& name : hashes) {

        if (eckit::HashFactory::instance().has(name)) {

            std::cout << name << std::endl;

            std::unique_ptr<eckit::Hash> hash(eckit::HashFactory::instance().build(name));

            timeAdd<20, 1>(*hash, buffer, timer);
            timeCompute<5>(*hash, buffer2, timer);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char* argv[]) {
    return run_tests(argc, argv);
}
