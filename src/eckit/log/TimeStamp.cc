// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/log/TimeStamp.h"

#include <cstdint>
#include <iomanip>
#include <sstream>

#include "eckit/eckit.h"
#include "eckit/utils/Clock.h"


namespace eckit {

const char* TimeStamp::defaultFormat_ = "%Y-%m-%d %H:%M:%S";

TimeStamp::TimeStamp(const std::string& format) : time_(Clock::now()), format_(format) {}

TimeStamp::TimeStamp(time_t t, const std::string& format) : time_(t), format_(format) {}

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
