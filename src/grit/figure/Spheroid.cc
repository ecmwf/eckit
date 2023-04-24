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
    NOTIMP;
}


double Spheroid::angle(const PointXYZ& A, const PointXYZ& B) const {
    NOTIMP;
}


double Spheroid::distance(const PointLatLon& A, const PointLatLon& B) const {
    NOTIMP;
}


double Spheroid::distance(const PointXYZ& A, const PointXYZ& B) const {
    NOTIMP;
}


double Spheroid::area() const {
    NOTIMP;
}


PointXYZ Spheroid::ll_to_xyz(const PointLatLon& P, double height) const {
    NOTIMP;
}


PointLatLon Spheroid::xyz_to_ll(const PointXYZ& P) const {
    NOTIMP;
}


}  // namespace grit::figure
