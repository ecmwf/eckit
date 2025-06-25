/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <string>

#include "eckit/maths/FloatingPointExceptions.h"
#include "eckit/runtime/Main.h"


int main(int argc, char** argv) {
    eckit::Main::initialise(argc, argv);

    using FPE = eckit::maths::FloatingPointExceptions;

    if (argc > 1) {
        FPE::enable_floating_point_exceptions(argv[1]);
        FPE::test(argv[1]);  // should raise a signal
    }
    else {
        FPE::enable_floating_point_exceptions();
        FPE::test("FE_DIVBYZERO");
    }

    return 1;
}
