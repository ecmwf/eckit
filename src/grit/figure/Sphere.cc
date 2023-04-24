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


#include "grit/figure/Sphere.h"

#include "grit/exception.h"


namespace grit::figure {


Sphere::Sphere(double R) : R_(R) {
    ASSERT(0. < R);
}


double Sphere::angle(const PointLatLon& A, const PointLatLon& B) const {
    NOTIMP;
}


double Sphere::angle(const PointXYZ& A, const PointXYZ& B) const {
    NOTIMP;
}


double Sphere::distance(const PointLatLon& A, const PointLatLon& B) const {
    NOTIMP;
}


double Sphere::distance(const PointXYZ& A, const PointXYZ& B) const {
    NOTIMP;
}


double Sphere::area() const {
    NOTIMP;
}


PointXYZ Sphere::ll_to_xyz(const PointLatLon& P, double height) const {
    NOTIMP;
}


PointLatLon Sphere::xyz_to_ll(const PointXYZ& P) const {
    NOTIMP;
}


}  // namespace grit::figure
