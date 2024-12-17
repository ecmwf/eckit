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


#include "eckit/geo/Shape.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Spec.h"


namespace eckit::geo {


Shape Shape::make_from_spec(const Spec& spec) {
    if (std::vector<value_type> shape; spec.get("shape", shape) && shape.size() == 2) {
        return {shape[0], shape[1]};
    }

    if (value_type nx = 0, ny = 0;
        (spec.get("nlon", nx) && spec.get("nlat", ny)) || (spec.get("nlon", nx) && spec.get("nlat", ny))) {
        return {nx, ny};
    }

    throw SpecNotFound("'shape' = ['nlon', 'nlat'] = ['nx', 'ny'] expected", Here());
}


Shape::Shape(value_type nx, value_type ny) : array{nx, ny} {
    if (!(nx > 0) || !(ny > 0)) {
        throw BadValue("'shape' = ['nlon', 'nlat'] = ['nx', 'ny'] > 0 expected", Here());
    }
}


}  // namespace eckit::geo
