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
#include <iomanip>
#include <cstdint>

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

    if (x.format_ == "hex") {
        s << std::setw(16) << std::setfill('0') << std::hex << static_cast<std::uint64_t>(x.time_);
        return s;
    }

    char buf[80];
#if eckit_HAVE_GMTIME_R
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
