/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/maths/FloatingPointExceptions.h"
#include "eckit/testing/Test.h"


namespace eckit::test {


using FPE = maths::FloatingPointExceptions;

std::vector<std::string> CODES{"FE_INVALID", "FE_INEXACT", "FE_DIVBYZERO", "FE_OVERFLOW", "FE_UNDERFLOW"};


CASE("invalid exception code") {
    EXPECT_THROWS_AS(FPE::enable_floating_point_exceptions("?"), UserError);
}


CASE("disabled exceptions") {
    FPE::disable_floating_point_exceptions();
    FPE::disable_custom_signal_handlers();

    for (const auto& code : CODES) {
        // exceptions are not enabled
        FPE::enable_custom_signal_handlers(true);
        FPE::test(code);

        FPE::disable_custom_signal_handlers();
        FPE::test(code);

        // can call twice
        FPE::disable_floating_point_exceptions(code);
        FPE::disable_floating_point_exceptions(code);
    }
}


CASE("can call twice") {
    SECTION("enable_floating_point_exceptions") {
        FPE::enable_floating_point_exceptions();
        FPE::enable_floating_point_exceptions();
    }

    SECTION("disable_floating_point_exceptions") {
        FPE::disable_floating_point_exceptions();
        FPE::disable_floating_point_exceptions();
    }

    SECTION("enable_custom_signal_handlers") {
        FPE::enable_custom_signal_handlers(true);
        FPE::enable_custom_signal_handlers(true);
    }

    SECTION("disable_custom_signal_handlers") {
        FPE::disable_custom_signal_handlers();
        FPE::disable_custom_signal_handlers();
    }
}


CASE("default exceptions") {
    FPE::disable_floating_point_exceptions();
    FPE::disable_custom_signal_handlers();

    SECTION("custom signal handlers are not installed") {
        for (const auto& code : CODES) {
            FPE::test(code);
        }
    }

    SECTION("custom signal handlers are not installed") {
        FPE::disable_custom_signal_handlers();
        FPE::enable_custom_signal_handlers();

        for (const auto& code : CODES) {
            FPE::test(code);
        }
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
