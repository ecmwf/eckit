// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <iostream>
#include <string>

#include "eckit/maths/FloatingPointExceptions.h"
#include "eckit/runtime/Main.h"


int main(int argc, char** argv) {
    eckit::Main::initialise(argc, argv);

    using FPE = eckit::maths::FloatingPointExceptions;

    if (argc > 1) {
        FPE::enable_floating_point_exceptions(argv[1]);
        FPE::enable_custom_signal_handlers();
        FPE::test(argv[1]);  // should raise a signal
    }
    else {
        FPE::enable_floating_point_exceptions();
        FPE::enable_custom_signal_handlers();
        FPE::test("FE_DIVBYZERO");
    }
    if (argc > 1) {
        std::cout << "Could not trap " << argv[1] << std::endl;
    }
    else {
        std::cout << "Could not trap FE_DIVBYZERO" << std::endl;
    }
    return 1;
}
