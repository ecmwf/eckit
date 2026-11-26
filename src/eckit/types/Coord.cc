// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/persist/DumpLoad.h"
#include "eckit/types/Coord.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Longitude::Longitude(double l) : Coord(l) {
    while (value_ < 0.0) {
        value_ += 360.0;
    }
    while (value_ >= 360.0) {
        value_ -= 360.0;
    }
}

Latitude::Latitude(double l) : Coord(l) {
    ASSERT(l <= 90.0 && l >= -90.0);
}

void Coord::print(std::ostream& s) const {
    s << value_;
}

void Coord::dump(DumpLoad& a) const {
    a.dump(value_);
}

void Coord::load(DumpLoad& a) {
    a.load(value_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
