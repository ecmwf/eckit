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


CASE("invalid exception") {
    EXPECT_THROWS_AS(FPE::enable_floating_point_exceptions("?"), UserError);
}


CASE("disabled exceptions") {
    for (const auto& code : CODES) {
        FPE::enable_floating_point_exceptions(code);

        // FPE::test("FE_DIVBYZERO");  // should trigger a signel

        FPE::disable_floating_point_exceptions(code);
        FPE::test(code);  // should not trigger a signel
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
