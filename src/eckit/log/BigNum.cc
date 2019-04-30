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

#include "eckit/log/BigNum.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

void BigNum::print(std::ostream& s, long long v) {
    if (v >= 1000) {
        print(s, v / 1000);
        s << ',';
        s << std::setw(3) << std::setfill('0') << (v % 1000);
    }
    else
        s << v;
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
