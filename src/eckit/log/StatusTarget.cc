// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/StatusTarget.h"

#include "eckit/runtime/Monitor.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

StatusTarget::StatusTarget() {}

void StatusTarget::line(const char* line) {
    Monitor::instance().status(line);
}

void StatusTarget::print(std::ostream& s) const {
    s << "StatusTarget()";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
