/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "ETA.h"

#include <iomanip>
#include <sstream>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ETA::ETA(double ETA) :
    ETA_(ETA) {}

ETA::ETA(const ::timeval& time) :
    ETA_(time.tv_sec + time.tv_usec / 1000000.0) {}

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
