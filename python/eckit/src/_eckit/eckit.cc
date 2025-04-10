/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/runtime/Main.h"


void eckit_main_initialise() {
    if (!eckit::Main::ready()) {
        static const int argc = 1;
        static char* argv[]   = {const_cast<char*>("eckit_main_initialise"), nullptr};

        eckit::Main::initialise(argc, argv);
    }
}
