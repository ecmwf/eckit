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


#include "eckit/geo/projection/Rotation.h"

#include <cmath>
#include <utility>

#include "eckit/config/Configuration.h"
#include "eckit/geo/UnitSphere.h"
#include "eckit/geo/util.h"
#include "eckit/maths/Matrix3.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static ProjectionBuilder<Rotation> __projection("rotation");


Rotation::Rotation() :
    Rotation(-90., 0., 0.) {}


Rotation::Rotation(double south_pole_lon, double south_pole_lat, double angle) :
    rotated_(true) {
    using M = maths::Matrix3<double>;

    struct NonRotated final : Rotate {
        PointLonLat operator()(const PointLonLat& p) const override { return p; }
    };

    struct RotationAngle final : Rotate {
        explicit RotationAngle(double angle) :
            angle_(angle) {}
        PointLonLat operator()(const PointLonLat& p) const override { return {p.lon + angle_, p.lat}; }
        const double angle_;
    };

    struct RotationMatrix final : Rotate {
        explicit RotationMatrix(M&& R) :
            R_(R) {}
        PointLonLat operator()(const PointLonLat& p) const override {
            return UnitSphere::convertCartesianToSpherical(R_ * UnitSphere::convertSphericalToCartesian(p));
        }
        const M R_;
    };

    constexpr auto eps = 1e-12;

    const auto alpha = util::degree_to_radian * angle;
    const auto theta = util::degree_to_radian * -(south_pole_lat + 90.);
    const auto phi   = util::degree_to_radian * -south_pole_lon;

    const auto ca = std::cos(alpha);
    const auto ct = std::cos(theta);
    const auto cp = std::cos(phi);

    if (types::is_approximately_equal(ct, 1., eps)) {
        angle    = PointLonLat::normalise_angle_to_minimum(angle - south_pole_lon, -180.);
        rotated_ = !types::is_approximately_equal(angle, 0., eps);

        fwd_.reset(rotated_ ? static_cast<Rotate*>(new RotationAngle(-angle)) : new NonRotated);
        inv_.reset(rotated_ ? static_cast<Rotate*>(new RotationAngle(angle)) : new NonRotated);
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
    fwd_ = std::make_unique<RotationMatrix>(M{ca * cp * ct - sa * sp,
                                              sa * cp * ct + ca * sp,
                                              cp * st,  //
                                              -sa * cp - ca * ct * sp,
                                              ca * cp - sa * ct * sp,
                                              -sp * st,  //
                                              -ca * st,
                                              -sa * st,
                                              ct});

    // Un-rotate (rotate by -φ, -ϑ, -α):
    // p = Ra Ry Rz q = [ cosα -sinα   ] [ cosϑ   -sinϑ ] [ cosφ -sinφ   ] q
    //                  [ sinα  cosα   ] [      1       ] [ sinφ  cosφ   ]
    //                  [            1 ] [ sinϑ    cosϑ ] [            1 ]
    inv_ = std::make_unique<RotationMatrix>(M{ca * cp * ct - sa * sp,
                                              -sa * cp - ca * ct * sp,
                                              -ca * st,  //
                                              sa * cp * ct + ca * sp,
                                              ca * cp - sa * ct * sp,
                                              -sa * st,  //
                                              cp * st,
                                              -sp * st,
                                              ct});
}


Rotation::Rotation(const Configuration& config) :
    Rotation(config.getDouble("south_pole_lon"), config.getDouble("south_pole_lat"), config.getDouble("angle", 0)) {}


}  // namespace eckit::geo::projection
