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
#include "eckit/geo/Spec.h"


namespace eckit::geo::figure {


Sphere::Sphere(double R) : R_(R) {
    if (!(0. < R_)) {
        throw BadValue("Sphere::R requires 0 < R", Here());
    }
}


Sphere::Sphere(const Spec& spec) : Sphere(spec.get_double("R")) {}


}  // namespace eckit::geo::figure
