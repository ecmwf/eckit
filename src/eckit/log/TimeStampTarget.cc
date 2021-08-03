/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iomanip>
#include <iostream>


#include "eckit/log/TimeStamp.h"
#include "eckit/log/TimeStampTarget.h"
#include "eckit/runtime/Monitor.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------


TimeStampTarget::TimeStampTarget(const char* tag, LogTarget* target) :
    WrapperTarget(target), tag_(tag) {}

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
