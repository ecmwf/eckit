// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Range.h"

#include "eckit/geo/Exceptions.h"


namespace eckit::geo {


double Range::min() const {
    auto _a = a();
    auto _b = b();
    return _a < _b ? _a : _b;
}


double Range::max() const {
    auto _a = a();
    auto _b = b();
    return _a < _b ? _b : _a;
}


Fraction Range::increment() const {
    throw exception::GridError("Range::increment not implemented", Here());
}


bool Range::periodic() const {
    throw exception::GridError("Range::periodic not implemented", Here());
}


bool Range::includesNorthPole() const {
    throw exception::GridError("Range::includesNorthPole not implemented", Here());
}


bool Range::includesSouthPole() const {
    throw exception::GridError("Range::includesSouthPole not implemented", Here());
}


}  // namespace eckit::geo
