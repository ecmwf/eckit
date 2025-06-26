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
#include <string>

#include "eckit/maths/FloatingPointExceptions.h"
#include "eckit/runtime/Main.h"


int main(int argc, char** argv) {
    eckit::Main::initialise(argc, argv);

    using FPE = eckit::maths::FloatingPointExceptions;

    std::string except{"FE_DIVBYZERO"};
    if (argc > 1) {
        except = argv[1];
    }
    FPE::enable_floating_point_exceptions(except);
    FPE::enable_custom_signal_handlers();
    FPE::test(except);  // should raise a signal

    std::cout << "Could not trap " << except << std::endl;
    return 1;
}
