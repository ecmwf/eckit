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

#include <cassert>


namespace grit::figure {


double Spheroid::angle(const PointLatLon& A, const PointLatLon& B) const {
    assert(false && "Not implemented: Spheroid::angle");
}


double Spheroid::angle(const Point3& A, const Point3& B) const {
    assert(false && "Not implemented: Spheroid::angle");
}


double Spheroid::distance(const PointLatLon& A, const PointLatLon& B) const {
    assert(false && "Not implemented: Spheroid::distance");
}


double Spheroid::distance(const Point3& A, const Point3& B) const {
    assert(false && "Not implemented: Spheroid::distance");
}


double Spheroid::area() const {
    assert(false && "Not implemented: Spheroid::area");
}


Point3 Spheroid::ll_to_xyz(const PointLatLon& P, double height) const {
    assert(false && "Not implemented: Spheroid::ll_to_xyz");
}


PointLatLon Spheroid::xyz_to_ll(const Point3& P) const {
    assert(false && "Not implemented: Spheroid::xyz_to_ll");
}


}  // namespace grit::figure
