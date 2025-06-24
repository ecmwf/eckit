/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// #include <cfenv>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/maths/FloatingPointExceptions.h"
#include "eckit/testing/Test.h"


namespace eckit::test {


using FPE = maths::FloatingPointExceptions;

std::vector<std::string> CODES{"FE_INVALID", "FE_INEXACT", "FE_DIVBYZERO", "FE_OVERFLOW", "FE_UNDERFLOW"};


CASE("invalid exception") {
    EXPECT_THROWS_AS(FPE::enable_floating_point_exceptions("?"), UserError);
}


CASE("enabling exceptions") {
    std::string default_fpe     = "FE_DIVBYZERO";
    std::string non_default_fpe = "FE_OVERFLOW";

    FPE::disable_floating_point_exceptions();

    EXPECT(FPE::excepts() == 0);
    EXPECT(FPE::excepts_as_string().empty());

    FPE::enable_floating_point_exceptions();

    EXPECT(FPE::excepts() != 0);
    ASSERT(FPE::excepts_as_string().find(default_fpe) != std::string::npos);
    ASSERT(FPE::excepts_as_string().find(non_default_fpe) != std::string::npos);

    FPE::enable_floating_point_exceptions(non_default_fpe);

    EXPECT(FPE::excepts() != 0);
    EXPECT(FPE::excepts_as_string().find(default_fpe) != std::string::npos);
    EXPECT(FPE::excepts_as_string().find(non_default_fpe) != std::string::npos);

    FPE::disable_floating_point_exceptions(default_fpe);

    EXPECT(FPE::excepts() != 0);
    EXPECT(FPE::excepts_as_string().find(default_fpe) == std::string::npos);
    EXPECT(FPE::excepts_as_string().find(non_default_fpe) != std::string::npos);

    FPE::disable_floating_point_exceptions();

    EXPECT(FPE::excepts() == 0);
    EXPECT(FPE::excepts_as_string().empty());
}


CASE("disabled exceptions") {
    for (const auto& code : CODES) {
        FPE::enable_floating_point_exceptions(code);
        FPE::enable_custom_signal_handlers();

        // FPE::test(FPE::excepts());  // should raise a signel

        FPE::disable_floating_point_exceptions(code);
        FPE::test(FPE::excepts());  // should not raise a signel

        FPE::disable_custom_signal_handlers();
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
