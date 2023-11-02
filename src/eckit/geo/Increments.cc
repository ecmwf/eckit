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

#include "eckit/config/Configuration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


static Increments make_from_config(const Configuration& config) {
    auto grid = config.getDoubleVector("grid");

    ASSERT_MSG(grid.size() == 2, "Increments: expected list of size 2");
    return {grid[0], grid[1]};
}


Increments::Increments(double west_east, double south_north) :
    array{west_east, south_north} {
    ASSERT(!types::is_equal(operator[](0), 0.));
    ASSERT(!types::is_equal(operator[](1), 0.));
}


Increments::Increments(const Configuration& config) :
    Increments(make_from_config(config)) {}


bool Increments::operator==(const Increments& other) const {
    return west_east == other.west_east && south_north == other.south_north;
}


}  // namespace eckit::geo
