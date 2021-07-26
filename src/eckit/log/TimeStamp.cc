/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>

#include "eckit/eckit.h"

#include "eckit/log/TimeStamp.h"
#include "eckit/utils/Clock.h"


namespace eckit {

const char* TimeStamp::defaultFormat_ = "%Y-%m-%d %H:%M:%S";

TimeStamp::TimeStamp(const std::string& format) :
    time_(Clock::now()), format_(format) {}

TimeStamp::TimeStamp(time_t t, const std::string& format) :
    time_(t), format_(format) {}

std::ostream& operator<<(std::ostream& s, const TimeStamp& x) {
    char buf[80];
#ifdef eckit_HAVE_GMTIME_R
    struct tm t;
    ::strftime(buf, sizeof(buf), x.format_.c_str(), gmtime_r(&x.time_, &t));
#else
    ::strftime(buf, sizeof(buf), x.format_.c_str(), gmtime(&x.time_));
#endif

    s << buf;

    return s;
}

TimeStamp::operator std::string() const {
    std::ostringstream s;
    s << *this;
    return s.str();
}


}  // namespace eckit
