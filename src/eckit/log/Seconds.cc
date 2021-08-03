/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <iostream>
#include <sstream>

#include "eckit/log/Seconds.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Seconds::Seconds(double seconds, bool compact) :
    seconds_(seconds), compact_(compact) {}

Seconds::Seconds(const ::timeval& time, bool compact) :
    seconds_(time.tv_sec + time.tv_usec / 1000000.0), compact_(compact) {}

static struct {
    int length_;
    const char* name_;
    const char shortName_;
} periods[] = {
    {
        7 * 24 * 60 * 60,
        "week",
        'w',
    },
    {
        24 * 60 * 60,
        "day",
        'd',
    },
    {
        60 * 60,
        "hour",
        'h',
    },
    {
        60,
        "minute",
        'm',
    },
    {
        1,
        "second",
        's',
    },
    {
        0,
        0,
        0,
    },
};

std::ostream& operator<<(std::ostream& s, const Seconds& sec) {

    // Use an intermediate std::stringstream, so that the entire
    // grouping can be manipulated with std::setw(), ...

    std::ostringstream ss;
    double t = sec.seconds_;
    long n   = t;
    int flg  = 0;

    for (int i = 0; periods[i].length_; i++) {
        long m = n / periods[i].length_;
        if (sec.compact_) {
            if (m || flg) {
                ss << m << periods[i].shortName_;
                n %= periods[i].length_;
                flg++;
            }
        }
        else {
            if (m) {
                if (flg)
                    ss << ' ';
                ss << m << ' ' << periods[i].name_;
                if (m > 1)
                    ss << 's';
                n %= periods[i].length_;
                flg++;
            }
        }
    }

    if (!flg) {
        ss << t << (sec.compact_ ? "s" : " second");
    }

    s << ss.str();

    return s;
}

Seconds::operator std::string() const {
    std::ostringstream s;
    s << *this;
    return s.str();
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
