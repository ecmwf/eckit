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


#include "eckit/geo/Increments.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Spec.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


Increments Increments::make_from_spec(const Spec& spec) {
    value_type x = 0;
    value_type y = 0;
    if (std::vector<value_type> value; spec.get("increments", value) || spec.get("grid", value) && value.size() == 2) {
        x = value[0];
        y = value[1];
    }
    else if (!spec.get("dx", x) || !spec.get("dy", y)) {
        throw SpecNotFound("'increments' = ['dx', 'dy'] expected", Here());
    }

    return {x, y};
}


Increments::Increments(value_type dx, value_type dy) : array{dx, dy} {
    if (!(dx != 0) || !(dy != 0)) {
        throw BadValue("'shape' = ['dx', 'dy'] != 0 expected", Here());
    }
}


bool Increments::operator==(const Increments& other) const {
    return types::is_approximately_equal(dx, other.dx) && types::is_approximately_equal(dy, other.dy);
}

}  // namespace eckit::geo
