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


#include "grit/projection/Rotation.h"

#include <cmath>

#include "grit/exception.h"
#include "grit/geometry/Sphere.h"
#include "grit/util.h"


namespace grit::projection {


Rotation::Rotation(double south_pole_lat, double south_pole_lon, double angle) :
    angle_(util::normalise_longitude_to_minimum(angle, 0.)) {
    const auto theta = -(south_pole_lat + 90.) * util::degrees_to_radians;
    const auto phi   = -(south_pole_lon)*util::degrees_to_radians;

    const auto sin_theta = std::sin(theta);
    const auto cos_theta = std::cos(theta);
    const auto sin_phi   = std::sin(phi);
    const auto cos_phi   = std::cos(phi);

    auto eq = [](double a, double b) { return util::is_approximately_equal(a, b, 1.e-12); };
    if (eq(sin_theta, 0) && eq(cos_theta, 1)) {
        rotation_ = eq(angle_, 0) && eq(sin_phi, 0) && eq(cos_phi, 1) ? rotation_type::UNROTATED : rotation_type::ANGLE;
        return;
    }

    rotation_ = rotation_type::ANGLE_VECTOR;

    // Rotate: rotate by ϑ (y-axis, along the rotated Greenwich meridian), then by φ (z-axis)
    // q = Rz * Ry * p = [  cosφ sinφ   ] * [  cosϑ   sinϑ ] * p = [  cosϑ.cosφ sinφ  sinϑ.cosφ ] * p
    //                   [ -sinφ cosφ   ]   [       1      ]       [ -cosϑ.sinφ cosφ -sinϑ.sinφ ]
    //                   [            1 ]   [ -sinϑ   cosϑ ]       [   -sinϑ            cosϑ    ]
    R_ = {cos_theta * cos_phi,  sin_phi, sin_theta * cos_phi,
          -cos_theta * sin_phi, cos_phi, -sin_theta * sin_phi,
          -sin_theta,           0.,      cos_theta};

    // Un-rotate (the reverse):
    // p = Ry * Rz * q = [ cosϑ   -sinϑ ] * [ cosφ -sinφ   ] * q = [ cosϑ.cosφ -cosϑ.sinφ -sinϑ ] * q
    //                   [      1       ]   [ sinφ  cosφ   ]       [    sinφ       cosφ         ]
    //                   [ sinϑ    cosϑ ]   [            1 ]       [ sinϑ.cosφ -sinϑ.sinφ  cosϑ ]
    U_ = {cos_theta * cos_phi, -cos_theta * sin_phi, -sin_theta, sin_phi, cos_phi, 0.,
          sin_theta * cos_phi, -sin_theta * sin_phi, cos_theta};
}


PointLatLon Rotation::fwd(const PointLatLon& p) const {
    if (rotation_ == rotation_type::ANGLE) {
        auto q = p;
        q.lon -= angle_;
        return q;
    }

    if (rotation_ == rotation_type::ANGLE_VECTOR) {
        auto q = p;
        q.lon -= angle_;
        return geometry::Sphere::xyz_to_ll(1., R_ * geometry::Sphere::ll_to_xyz(1., q, 0.));
    }

    return p;
}


PointLatLon Rotation::inv(const PointLatLon& q) const {
    if (rotation_ == rotation_type::ANGLE) {
        auto p = q;
        p.lon += angle_;
        return p;
    }

    if (rotation_ == rotation_type::ANGLE_VECTOR) {
        auto p = geometry::Sphere::xyz_to_ll(1., U_ * geometry::Sphere::ll_to_xyz(1., q, 0.));
        p.lon += angle_;
        return p;
    }

    return q;
}


Point Rotation::fwd(const Point& p) const {
    return fwd(std::get<PointLatLon>(p));
}


Point Rotation::inv(const Point& q) const {
    return inv(std::get<PointLatLon>(q));
}


}  // namespace grit::projection
