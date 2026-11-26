// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "ETA.h"

#include <iomanip>
#include <sstream>

#include "eckit/log/Seconds.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ETA::ETA(double ETA) : ETA_(ETA) {}

ETA::ETA(const ::timeval& time) : ETA_(time.tv_sec + (time.tv_usec / Seconds::usec_per_sec)) {}

std::ostream& operator<<(std::ostream& s, const ETA& sec) {
    double t = sec.ETA_;
    long n   = t;

    long hour = n / (60 * 60);
    n %= (60 * 60);
    long minutes = n / 60;
    n %= 60;

    s << hour << ':' << std::setfill('0') << std::setw(2) << minutes << ':' << std::setfill('0') << std::setw(2) << n
      << std::setfill(' ');

    return s;
}

ETA::operator std::string() const {
    std::ostringstream s;
    s << *this;
    return s.str();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
