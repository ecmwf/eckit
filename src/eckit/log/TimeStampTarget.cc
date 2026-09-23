// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/TimeStampTarget.h"

#include <iomanip>
#include <iostream>

#include "eckit/log/TimeStamp.h"
#include "eckit/runtime/Monitor.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


TimeStampTarget::TimeStampTarget(const char* tag, LogTarget* target) : WrapperTarget(target), tag_(tag) {}

TimeStampTarget::~TimeStampTarget() {}

void TimeStampTarget::writePrefix() {

    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << Monitor::instance().self() << std::setfill(' ') << ' ' << TimeStamp()
        << ' ';

    if (tag_ && *tag_) {
        oss << tag_ << ' ';
    }

    std::string s = oss.str();
    const char* p = s.c_str();
    target_->write(p, p + s.size());
}


void TimeStampTarget::writeSuffix() {}

void TimeStampTarget::print(std::ostream& s) const {
    s << "TimeStampTarget(";
    if (target_) {
        s << "target=" << *target_ << ")";
    }
    s << ")";
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
