// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <iomanip>
#include <iostream>

#include "eckit/log/BigNum.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void BigNum::print(std::ostream& s, long long v) {
    if (v >= 1000) {
        print(s, v / 1000);
        s << ',';
        s << std::setw(3) << std::setfill('0') << (v % 1000);
    }
    else {
        s << v;
    }
}

void BigNum::print(std::ostream& s) const {
    long long v = value_;
    if (v < 0) {
        v = -v;
        s << '-';
    }
    char oldfill = s.fill();
    print(s, v);
    s << std::setfill(oldfill);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
