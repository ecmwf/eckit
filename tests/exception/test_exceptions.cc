/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/runtime/Tool.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

std::string evaluate_message(bool assser_is_correct) {
    if(assser_is_correct) {
        // this shoudl never execute, and therefore the code below never reached
        throw NotImplemented(Here());
        return "NON REACHABLE CODE";
    }
    else {
        std::cout << "THIS SHOULD PRINT" << std::endl;
        return "ASSERT FAILS";
    }
}

CASE("Test ASSERT throws correctly") {

    bool notfail = true;
    EXPECT_NO_THROW(ASSERT(notfail));

    notfail = false;
    EXPECT_THROWS_AS(ASSERT(notfail), AssertionFailed);
}

CASE("Test ASSERT_MSG does not evaluate message if assertion not triggered") {

    bool notfail = true;
    EXPECT_NO_THROW(ASSERT_MSG(notfail, evaluate_message(notfail)));

    notfail = false;
    EXPECT_THROWS_AS(ASSERT_MSG(notfail, evaluate_message(notfail)), AssertionFailed);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
