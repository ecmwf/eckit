// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <cstdlib>

#include "eckit/log/BigNum.h"
#include "eckit/log/Timer.h"
#include "eckit/types/FloatCompare.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

void compare(size_t n) {
    for (size_t i = 0; i < n; ++i) {
        double x = (double)::rand() / (double)RAND_MAX;
        eckit::types::is_approximately_equal(x, double(0.5));
    }
}

//----------------------------------------------------------------------------------------------------------------------

CASE("TestDoubleCompareSpeed") {
    const size_t n = 30000000;  // with this data set, on a modern cpu we expect > 25E6 /s

    eckit::Timer t;

    compare(n);

    eckit::Log::info() << "Double compare speed: " << eckit::BigNum(n / t.elapsed()) << " /s" << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
