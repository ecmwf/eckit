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


#include "eckit/geo/figure/Sphere.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::figure {


Sphere::Sphere(double R) : R_(R) {
    ASSERT_MSG(types::is_strictly_greater(R_, 0.), "Sphere requires R > 0");
}


Sphere::Sphere(const Spec& spec) : Sphere(spec.get_double("R")) {}


void Sphere::spec(spec::Custom& custom) const {
    custom.set("R", R_);
}


}  // namespace eckit::geo::figure
