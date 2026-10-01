// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/log/Progress.h"
#include "eckit/runtime/Monitor.h"


//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Progress::Progress(const std::string& name, unsigned long long min, unsigned long long max) {
    Monitor::instance().progress(name, min, max);
}

Progress::~Progress() {
    Monitor::instance().progress();
}

void Progress::operator()(unsigned long long value) {
    Monitor::instance().progress(value);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
