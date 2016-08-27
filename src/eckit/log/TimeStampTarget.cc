/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/log/TimeStampTarget.h"
#include "eckit/runtime/Monitor.h"
#include "eckit/log/TimeStamp.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------


TimeStampTarget::TimeStampTarget(LogTarget* target):
    WrapperTarget(target)
{

}

void TimeStampTarget::writePrefix() {

    std::ostringstream oss;
    oss << std::setw(3)
        << std::setfill('0')
        << Monitor::instance().self()
        << std::setfill(' ') << ' '
        << TimeStamp() << ' ';


    std::string s = oss.str();
    const char* p = s.c_str();
    target_->write(p, p + s.size());
}


void TimeStampTarget::writeSuffix() {
}

//-----------------------------------------------------------------------------

} // namespace eckit
