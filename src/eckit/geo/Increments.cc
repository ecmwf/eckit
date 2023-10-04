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


namespace eckit::geo {


Increments::Increments(double west_east, double south_north) :
    P{west_east, south_north} {
    ASSERT(operator[](0) > 0.);
    ASSERT(operator[](1) > 0.);
}


Increments::Increments(const Configuration& config) :
    Increments(VectorHelper(config)) {}


Increments::Increments(const VectorHelper& helper) :
    Increments(helper[0], helper[1]) {}


Increments::VectorHelper::VectorHelper(const Configuration& config) :
    vector(config.getDoubleVector("grid")) {
    ASSERT(size() == 2);
}


}  // namespace eckit::geo
