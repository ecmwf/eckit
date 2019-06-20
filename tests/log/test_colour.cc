/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>

#include "eckit/log/Colour.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"

#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test_colour") {

    Log::info() << "Hello, worlds" << std::endl;

    std::cout << Colour::red << "Red" << Colour::reset << std::endl;
    std::cout << Colour::off;
    std::cout << Colour::red << "Red" << Colour::reset << std::endl;
    std::cout << Colour::on;
    std::cout << Colour::blue << "Red" << Colour::reset << std::endl;
    std::cout << Colour::red << Colour::bold << "Red" << Colour::reset << std::endl;
    std::cout << Colour::yellow << Colour::underline << "Red" << Colour::reset << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
