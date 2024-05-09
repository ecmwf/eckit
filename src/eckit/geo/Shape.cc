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
    value_type x = 0;
    value_type y = 0;
    if (std::vector<value_type> value; spec.get("shape", value) && value.size() == 2) {
        x = value[0];
        y = value[1];
    }
    else if (!spec.get("nx", x) || !spec.get("ny", y)) {
        throw SpecNotFound("'shape' = ['nx', 'ny'] expected", Here());
    }

    return {x, y};
}


Shape::Shape(value_type nx, value_type ny) : array{nx, ny} {
    if (!(nx > 0) || !(ny > 0)) {
        throw BadValue("'shape' = ['nx', 'ny'] > 0 expected", Here());
    }
}


}  // namespace eckit::geo
