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

#include "eckit/testing/Test.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

std::string evaluate_message(bool answer_is_correct) {
    if (answer_is_correct) {
        // this should never execute, and therefore the code below never reached
        throw NotImplemented(Here());
// Suppress warning "statement is unreachable"
#if defined(__NVCOMPILER)
#pragma diag_suppress 111
#endif
        return "NON REACHABLE CODE";
    }

    std::cout << "THIS SHOULD PRINT" << std::endl;
    return "ASSERT FAILS";
}

CASE("Test AssertionFailed") {
    EXPECT_NO_THROW(ASSERT(true));
    EXPECT_THROWS_AS(ASSERT(false), AssertionFailed);
}

CASE("Test FunctionalityNotSupported") {
    struct A {
        virtual void method() {
            throw FunctionalityNotSupported("not supported");
        }
    } a;

    struct B final : A {
        void method() override {}
    } b;

    // "not supported" is also "not implemented"
    EXPECT_THROWS_AS(a.method(), FunctionalityNotSupported);
    EXPECT_THROWS_AS(a.method(), NotImplemented);

    EXPECT_NO_THROW(b.method());
}

CASE("Test ASSERT_MSG does not evaluate message if assertion not triggered") {

    bool notfail = true;
    EXPECT_NO_THROW(ASSERT_MSG(notfail, evaluate_message(notfail)));

    notfail = false;
    EXPECT_THROWS_AS(ASSERT_MSG(notfail, evaluate_message(notfail)), AssertionFailed);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
