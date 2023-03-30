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


namespace grit::figure {


double Spheroid::angle(const PointLatLon& A, const PointLatLon& B) const {
    NOTIMP;
}


double Spheroid::angle(const Point3& A, const Point3& B) const {
    NOTIMP;
}


double Spheroid::distance(const PointLatLon& A, const PointLatLon& B) const {
    NOTIMP;
}


double Spheroid::distance(const Point3& A, const Point3& B) const {
    NOTIMP;
}


double Spheroid::area() const {
    NOTIMP;
}


Point3 Spheroid::ll_to_xyz(const PointLatLon& P, double height) const {
    NOTIMP;
}


PointLatLon Spheroid::xyz_to_ll(const Point3& P) const {
    NOTIMP;
}


}  // namespace grit::figure
