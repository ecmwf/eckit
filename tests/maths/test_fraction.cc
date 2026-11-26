// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/Log.h"
#include "eckit/maths/FloatingPointExceptions.h"
#include "eckit/types/Fraction.h"

#include "eckit/testing/Test.h"

namespace eckit::test {


auto& LOG = Log::info();


CASE("Values known to have problematic conversion to fraction") {
    using FPE = maths::FloatingPointExceptions;
    FPE::enable_floating_point_exceptions();  // default exceptions only

    auto values = std::vector<double>{19.011363983154297, 0.47718059708975263};
    for (auto value : values) {

        LOG << "Test " << value << "..." << std::endl;

        auto frac = Fraction(value);
        LOG << "Test " << value << " = " << frac << std::endl;

        EXPECT_NOT_EQUAL(frac.denominator(), 0);
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    eckit::test::LOG.precision(16);

    return eckit::testing::run_tests(argc, argv);
}
