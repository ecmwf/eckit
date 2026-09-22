// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "eckit/log/Colour.h"
#include "eckit/log/Log.h"
#include "eckit/runtime/Tool.h"
#include "eckit/testing/Test.h"

using namespace std;
using namespace eckit;
using namespace eckit::testing;

namespace eckit::test {

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

}  // namespace eckit::test

int main(int argc, char** argv) {
    return run_tests(argc, argv);
}
