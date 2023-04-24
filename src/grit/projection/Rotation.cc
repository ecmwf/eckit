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
#include <utility>

#include "grit/geometry/Sphere.h"
#include "grit/types/MatrixXYZ.h"
#include "grit/util.h"


namespace grit::projection {


Rotation::Rotation(double south_pole_lat, double south_pole_lon, double angle) : rotated_(true) {
    using M = types::MatrixXYZ<double>;

    struct No final : Rotate {
        PointLatLon operator()(const PointLatLon& p) const override { return p; }
    };

    struct Angle final : Rotate {
        explicit Angle(double angle) : angle_(angle) {}
        PointLatLon operator()(const PointLatLon& p) const override { return {p.lat, p.lon + angle_}; }
        const double angle_;
    };

    struct Matrix final : Rotate {
        explicit Matrix(M&& R) : R_(R) {}
        PointLatLon operator()(const PointLatLon& p) const override {
            return geometry::Sphere::xyz_to_ll(1., R_ * geometry::Sphere::ll_to_xyz(1., p, 0.));
        }
        const M R_;
    };

    const auto alpha = angle * util::degrees_to_radians;
    const auto theta = -(south_pole_lat + 90.) * util::degrees_to_radians;
    const auto phi   = -(south_pole_lon)*util::degrees_to_radians;

    const auto ca = std::cos(alpha);
    const auto ct = std::cos(theta);
    const auto cp = std::cos(phi);

    if (util::is_approximately_equal(ct, 1., 1.e-12)) {
        angle    = util::normalise_longitude_to_minimum(angle - south_pole_lon, -180.);
        rotated_ = !util::is_approximately_equal(angle, 0., 1.e-12);

        fwd_.reset(rotated_ ? static_cast<Rotate*>(new Angle(-angle)) : new No);
        inv_.reset(rotated_ ? static_cast<Rotate*>(new Angle(angle)) : new No);
        return;
    }

    // FIXME this supports either angle-based or matrix-based rotation (but not both);
    // Implementing as Euler angles rotation matrix (ideal, but reordering Rz Ry Ra) changes the existing unit test

    const auto sa = std::sin(alpha);
    const auto st = std::sin(theta);
    const auto sp = std::sin(phi);

    // Rotate: rotate by α, then ϑ (y-axis, along the rotated Greenwich meridian), then φ (z-axis)
    // q = Rz Ry Ra p = [  cosφ sinφ   ] [  cosϑ   sinϑ ] [  cosα sinα   ] p
    //                  [ -sinφ cosφ   ] [       1      ] [ -sinα cosα   ]
    //                  [            1 ] [ -sinϑ   cosϑ ] [            1 ]
    fwd_ = std::make_unique<Matrix>(M{ca * cp * ct - sa * sp, sa * cp * ct + ca * sp, cp * st,    //
                                      -sa * cp - ca * ct * sp, ca * cp - sa * ct * sp, -sp * st,  //
                                      -ca * st, -sa * st, ct});

    // Un-rotate (rotate by -φ, -ϑ, -α):
    // p = Ra Ry Rz q = [ cosα -sinα   ] [ cosϑ   -sinϑ ] [ cosφ -sinφ   ] q
    //                  [ sinα  cosα   ] [      1       ] [ sinφ  cosφ   ]
    //                  [            1 ] [ sinϑ    cosϑ ] [            1 ]
    inv_ = std::make_unique<Matrix>(M{ca * cp * ct - sa * sp, -sa * cp - ca * ct * sp, -ca * st,  //
                                      sa * cp * ct + ca * sp, ca * cp - sa * ct * sp, -sa * st,   //
                                      cp * st, -sp * st, ct});
}


}  // namespace grit::projection
