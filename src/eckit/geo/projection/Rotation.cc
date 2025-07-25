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

#include "eckit/geo/figure/UnitSphere.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/maths/Matrix3.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static ProjectionRegisterType<Rotation> PROJECTION("rotation");


Rotation::Rotation(const Spec& spec) :
    Rotation(
        [](const auto& spec) -> PointLonLat {
            if (std::vector<double> r; spec.get("rotation", r)) {
                ASSERT_MSG(r.size() == 2, "Rotation: expected 'rotation' as a list of size 2");
                return {r[0], r[1]};
            }

            if (auto lon = SOUTH_POLE.lon, lat = SOUTH_POLE.lat;
                spec.get("south_pole_lon", lon) && spec.get("south_pole_lat", lat)) {
                return {lon, lat};
            }

            return SOUTH_POLE;
        }(spec),
        [](const auto& spec) -> double {
            double angle = 0.;
            spec.get("rotation_angle", angle);
            return angle;
        }(spec)) {}


Rotation::Rotation(const PointLonLat& south_pole, double angle) :
    south_pole_(south_pole), angle_(angle), rotated_(true) {
    using M = maths::Matrix3<double>;

    struct NonRotated final : Implementation {
        PointLonLat operator()(const PointLonLat& p) const override { return p; }
    };

    struct RotationAngle final : Implementation {
        explicit RotationAngle(double angle) : angle_(angle) {}
        PointLonLat operator()(const PointLonLat& p) const override { return {p.lon + angle_, p.lat}; }
        const double angle_;
    };

    struct RotationMatrix final : Implementation {
        explicit RotationMatrix(M&& R) : R_(R) {}
        PointLonLat operator()(const PointLonLat& p) const override {
            return figure::UnitSphere::_convertCartesianToSpherical(
                R_ * figure::UnitSphere::_convertSphericalToCartesian(p));
        }
        const M R_;
    };

    const auto alpha = util::DEGREE_TO_RADIAN * angle;
    const auto theta = util::DEGREE_TO_RADIAN * -(south_pole_.lat + 90.);
    const auto phi   = util::DEGREE_TO_RADIAN * -south_pole_.lon;

    const auto ca = std::cos(alpha);
    const auto ct = std::cos(theta);
    const auto cp = std::cos(phi);

    if (types::is_approximately_equal(ct, 1., PointLonLat::EPS * util::DEGREE_TO_RADIAN)) {
        angle_   = PointLonLat::normalise_angle_to_minimum(angle_ - south_pole_.lon, -PointLonLat::FLAT_ANGLE);
        rotated_ = !types::is_approximately_equal(angle_, 0., PointLonLat::EPS);

        fwd_.reset(rotated_ ? static_cast<Implementation*>(new RotationAngle(-angle)) : new NonRotated);
        inv_.reset(rotated_ ? static_cast<Implementation*>(new RotationAngle(angle)) : new NonRotated);
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
    fwd_ = std::make_shared<RotationMatrix>(M{ca * cp * ct - sa * sp, sa * cp * ct + ca * sp,
                                              cp * st,  //
                                              -sa * cp - ca * ct * sp, ca * cp - sa * ct * sp,
                                              -sp * st,  //
                                              -ca * st, -sa * st, ct});

    // Un-rotate (rotate by -φ, -ϑ, -α):
    // p = Ra Ry Rz q = [ cosα -sinα   ] [ cosϑ   -sinϑ ] [ cosφ -sinφ   ] q
    //                  [ sinα  cosα   ] [      1       ] [ sinφ  cosφ   ]
    //                  [            1 ] [ sinϑ    cosϑ ] [            1 ]
    inv_ = std::make_shared<RotationMatrix>(M{ca * cp * ct - sa * sp, -sa * cp - ca * ct * sp,
                                              -ca * st,  //
                                              sa * cp * ct + ca * sp, ca * cp - sa * ct * sp,
                                              -sa * st,  //
                                              cp * st, -sp * st, ct});

    angle_ = PointLonLat::normalise_angle_to_minimum(angle_, -PointLonLat::FLAT_ANGLE);
}


const std::string& Rotation::type() const {
    static const std::string type{"rotation"};
    return type;
}


Rotation* Rotation::make_from_spec(const Spec& spec) {
    double angle = 0.;
    spec.get("rotation_angle", angle);

    auto lon = SOUTH_POLE.lon;
    auto lat = SOUTH_POLE.lat;
    if (std::vector<double> r{lon, lat}; spec.get("rotation", r)) {
        ASSERT_MSG(r.size() == 2, "Rotation: expected 'rotation' as a list of size 2");
        lon = r[0];
        lat = r[1];
    }
    else {
        ASSERT_MSG(spec.get("south_pole_lon", lon) == spec.get("south_pole_lat", lat),
                   "Rotation: expected 'south_pole_lon' and 'south_pole_lat'");
    }

    auto* r = new Rotation{{lon, lat}, angle};
    if (!r->rotated()) {
        delete r;
        r = nullptr;
    }

    return r;
}


void Rotation::fill_spec(spec::Custom& custom) const {
    bool projection = false;

    if (!points_equal(SOUTH_POLE, south_pole_)) {
        custom.set("south_pole_lon", south_pole_.lon);
        custom.set("south_pole_lat", south_pole_.lat);
        projection = true;
    }

    if (!types::is_approximately_equal(angle_, 0., PointLonLat::EPS)) {
        custom.set("rotation_angle", angle_);
        projection = true;
    }

    if (projection) {
        custom.set("type", type());
    }
}


}  // namespace eckit::geo::projection
