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


#include "grit/figure/Spheroid.h"

#include "grit/exception.h"
#include "grit/geometry/Spheroid.h"
#include "grit/util.h"


namespace grit::figure {


Spheroid::Spheroid(double a, double b) : a_(a), b_(b) {
    ASSERT(0. < a);
    ASSERT(0. < b);
}


bool Spheroid::sphere() const {
    return util::is_approximately_equal(a_, b_);
}


double Spheroid::R() const {
    ASSERT_MSG("Spheroid::R", sphere());
    return a_;
}


double Spheroid::angle(const PointLatLon& A, const PointLatLon& B) const {
    return geometry::Spheroid::angle(A, B);
}


double Spheroid::angle(const PointXYZ& A, const PointXYZ& B) const {
    return geometry::Spheroid::angle(a_, b_, A, B);
}


double Spheroid::distance(const PointLatLon& A, const PointLatLon& B) const {
    return geometry::Spheroid::distance(a_, b_, A, B);
}


double Spheroid::distance(const PointXYZ& A, const PointXYZ& B) const {
    return geometry::Spheroid::distance(a_, b_, A, B);
}


double Spheroid::area() const {
    return geometry::Spheroid::area(a_, b_);
}


PointXYZ Spheroid::ll_to_xyz(const PointLatLon& P) const {
    return geometry::Spheroid::ll_to_xyz(a_, b_, P, 0.);
}


PointLatLon Spheroid::xyz_to_ll(const PointXYZ& P) const {
    return geometry::Spheroid::xyz_to_ll(a_, b_, P);
}


}  // namespace grit::figure
