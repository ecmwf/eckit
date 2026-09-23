// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/geo/Shape.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/spec/Spec.h"


namespace eckit::geo {


Shape Shape::make_from_spec(const spec::Spec& spec) {
    if (std::vector<value_type> shape; spec.get("shape", shape) && shape.size() == 2) {
        return {shape[0], shape[1]};
    }

    if (value_type nx = 0, ny = 0;
        (spec.get("nlon", nx) && spec.get("nlat", ny)) || (spec.get("nlon", nx) && spec.get("nlat", ny))) {
        return {nx, ny};
    }

    throw exception::SpecError("'shape' = ['nlon', 'nlat'] = ['nx', 'ny'] expected", Here());
}


Shape::Shape(value_type nx, value_type ny) : array{nx, ny} {
    if (!(nx > 0) || !(ny > 0)) {
        throw BadValue("'shape' = ['nlon', 'nlat'] = ['nx', 'ny'] > 0 expected", Here());
    }
}


}  // namespace eckit::geo
