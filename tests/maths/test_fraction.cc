/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/log/Log.h"
#include "eckit/maths/FloatingPointExceptions.h"
#include "eckit/types/Fraction.h"

#include "eckit/testing/Test.h"

namespace eckit::test {


auto& LOG = Log::info();


CASE("Values known to have problematic conversion to fraction") {
    using FPE = maths::FloatingPointExceptions;
    FPE::enable_floating_point_exceptions();  // default exceptions only
    FPE::enable_custom_signal_handlers();

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
