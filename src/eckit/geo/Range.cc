/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


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
