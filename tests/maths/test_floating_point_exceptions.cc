/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/exception/Exceptions.h"
#include "eckit/maths/FloatingPointExceptions.h"
#include "eckit/testing/Test.h"


namespace eckit::test {


CASE("invalid exception") {
    EXPECT_THROWS_AS(maths::FloatingPointExceptions::enable_floating_point_exceptions("?"), UserError);
}


CASE("disabled exceptions") {
    using FPE = maths::FloatingPointExceptions;

    struct Case {
        const std::string code;
        const std::function<void(void)>& raise;
    };

    for (const auto& fe : {
             Case{"FE_INVALID", FPE::test_invalid},      //
             Case{"FE_INEXACT", FPE::test_inexact},      //
             Case{"FE_DIVBYZERO", FPE::test_divbyzero},  //
             Case{"FE_OVERFLOW", FPE::test_overflow},    //
             Case{"FE_UNDERFLOW", FPE::test_underflow},  //
             // Case{"FE_DENORMAL", FPE::test_denormal},
         }) {
        maths::FloatingPointExceptions::disable_floating_point_exceptions();
        EXPECT_NO_THROW(fe.raise());
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
