// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/runtime/Main.h"


void eckit_main_initialise() {
    if (!eckit::Main::ready()) {
        static const int argc = 1;
        static char* argv[]   = {const_cast<char*>("eckit_main_initialise"), nullptr};

        eckit::Main::initialise(argc, argv);
    }
}
