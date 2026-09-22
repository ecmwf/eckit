// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/MonitorTarget.h"

#include <iostream>

#include "eckit/runtime/Monitor.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

MonitorTarget::MonitorTarget(LogTarget* target) : WrapperTarget(target) {}

MonitorTarget::~MonitorTarget() {}

void MonitorTarget::write(const char* start, const char* end) {
    if (start >= end)
        return;
    Monitor::instance().out(const_cast<char*>(start), const_cast<char*>(end));
    target_->write(start, end);
}

void MonitorTarget::print(std::ostream& s) const {
    s << "MonitorTarget()";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
