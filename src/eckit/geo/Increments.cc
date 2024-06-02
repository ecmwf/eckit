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
    if (std::vector<value_type> grid; (spec.get("increments", grid) || spec.get("grid", grid)) && grid.size() == 2) {
        return {grid[0], grid[1]};
    }

    if (value_type dx = 0, dy = 0; (spec.get("west_east_increment", dx) && spec.get("south_north_increment", dy))
                                   || (spec.get("dlon", dx) && spec.get("dlat", dy))
                                   || (spec.get("dx", dx) && spec.get("dy", dy))) {
        return {dx, dy};
    }

    throw SpecNotFound(
        "'increments' = 'grid' = ['dx', 'dy'] = ['west_east_increment', 'south_north_increment'] = ['dlon', 'dlat'] = "
        "['dx', 'dy'] expected",
        Here());
}


Increments::Increments(value_type dx, value_type dy) : array{dx, dy} {
    if (!(dx != 0) || !(dy != 0)) {
        throw BadValue(
            "'increments' = 'grid' = ['west_east_increment', 'south_north_increment'] = ['dlon', 'dlat'] = ['dx', "
            "'dy'] != 0 expected",
            Here());
    }
}


bool Increments::operator==(const Increments& other) const {
    return types::is_approximately_equal(dx, other.dx) && types::is_approximately_equal(dy, other.dy);
}

}  // namespace eckit::geo
