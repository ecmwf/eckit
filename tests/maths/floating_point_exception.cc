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
    const std::string code(argc > 1 ? argv[1] : "");

    using FPE = eckit::maths::FloatingPointExceptions;
    FPE::enable_floating_point_exceptions(code);

    code == "FE_INVALID"     ? FPE::test_invalid()
    : code == "FE_INEXACT"   ? FPE::test_inexact()
    : code == "FE_DIVBYZERO" ? FPE::test_divbyzero()
    : code == "FE_OVERFLOW"  ? FPE::test_overflow()
    : code == "FE_UNDERFLOW" ? FPE::test_underflow()
    : code == "FE_DENORMAL"  ? FPE::test_denormal()
                             : throw 1;

    return 1;
}
