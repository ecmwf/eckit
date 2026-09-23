// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/MessageTarget.h"

#include "eckit/runtime/Monitor.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

MessageTarget::MessageTarget() {}

void MessageTarget::line(const char* line) {
    Monitor::instance().message(line);
}

void MessageTarget::print(std::ostream& s) const {
    s << "MessageTarget()";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
