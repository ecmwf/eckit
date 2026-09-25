// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/Rotation.h"

#include <cmath>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/figure/UnitSphere.h"
#include "eckit/geo/util.h"
#include "eckit/maths/Matrix3.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static ProjectionRegisterType<Rotation> PROJECTION("rotation");


Rotation::Rotation(const Spec& spec) :
    Rotation(
        [](const auto& spec) -> PointLonLat {
            if (std::vector<double> p; spec.get("south_pole", p) || spec.get("rotation", p)) {
                ASSERT_MSG(p.size() == 2, "Rotation: expected 'south_pole' as a list of size 2");
                return {p[0], p[1]};
            }

            if (auto lon = SOUTH_POLE.lon(), lat = SOUTH_POLE.lat();
                spec.has("south_pole_lon") || spec.has("south_pole_lat")) {
                ASSERT_MSG(spec.get("south_pole_lon", lon) && spec.get("south_pole_lat", lat),
                           "Rotation: 'south_pole_lon' and 'south_pole_lat' are required together");
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
    Projection(nullptr, PointLonLat{}, PointLonLat{}), south_pole_(south_pole), angle_(angle), rotated_(true) {
    using M = maths::Matrix3<double>;

    struct NonRotated final : Implementation {
        using Implementation::operator();
        PointLonLat operator()(const PointLonLat& p) const override { return p; }
        std::vector<std::vector<double>> operator()(const std::vector<double>& lon,
                                                    const std::vector<double>& lat) const override {
            return {lon, lat};
        }
    };

    struct RotationAngle final : Implementation {
        explicit RotationAngle(double angle) : angle_(angle) {}
        using Implementation::operator();
        PointLonLat operator()(const PointLonLat& p) const override { return {p.lon() + angle_, p.lat()}; }
        std::vector<std::vector<double>> operator()(const std::vector<double>& lon,
                                                    const std::vector<double>& lat) const override {
            std::vector<std::vector<double>> out{lon, lat};
            for (auto& x : out[0]) {
                x += angle_;
            }
            return out;
        }
        const double angle_;
    };

    struct RotationMatrix final : Implementation {
        explicit RotationMatrix(M&& R) : R_(R) {}
        using Implementation::operator();
        PointLonLat operator()(const PointLonLat& p) const override {
            return figure::UnitSphere::_convertCartesianToSpherical(
                R_ * figure::UnitSphere::_convertSphericalToCartesian(p));
        }
        std::vector<std::vector<double>> operator()(const std::vector<double>& lon,
                                                    const std::vector<double>& lat) const override {
            const auto n = lon.size();

            // spherical to Cartesian (unit sphere)
            std::vector<double> x(n);
            std::vector<double> y(n);
            std::vector<double> z(n);
            for (size_t i = 0; i < n; ++i) {
                const auto p = figure::UnitSphere::_convertSphericalToCartesian(PointLonLat{lon[i], lat[i]});
                x[i]         = p.X();
                y[i]         = p.Y();
                z[i]         = p.Z();
            }

            // rotation, as the matrix product of all points at once
            for (size_t i = 0; i < n; ++i) {
                const auto a = x[i];
                const auto b = y[i];
                const auto c = z[i];
                x[i]         = R_.XX() * a + R_.XY() * b + R_.XZ() * c;
                y[i]         = R_.YX() * a + R_.YY() * b + R_.YZ() * c;
                z[i]         = R_.ZX() * a + R_.ZY() * b + R_.ZZ() * c;
            }

            // Cartesian to spherical
            std::vector<std::vector<double>> out{std::vector<double>(n), std::vector<double>(n)};
            for (size_t i = 0; i < n; ++i) {
                const auto q = figure::UnitSphere::_convertCartesianToSpherical(PointXYZ{x[i], y[i], z[i]});
                out[0][i]    = q.lon();
                out[1][i]    = q.lat();
            }
            return out;
        }
        const M R_;
    };

    const auto alpha = util::DEGREE_TO_RADIAN * angle;
    const auto theta = util::DEGREE_TO_RADIAN * -(south_pole_.lat() + 90.);
    const auto phi   = util::DEGREE_TO_RADIAN * -south_pole_.lon();

    const auto ca = std::cos(alpha);
    const auto ct = std::cos(theta);
    const auto cp = std::cos(phi);

    if (types::is_approximately_equal(ct, 1., PointLonLat::EPS * util::DEGREE_TO_RADIAN)) {
        angle_   = PointLonLat::normalise_angle_to_minimum(angle_ - south_pole_.lon(), -PointLonLat::FLAT_ANGLE);
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

    auto lon = SOUTH_POLE.lon();
    auto lat = SOUTH_POLE.lat();
    if (std::vector<double> p{lon, lat}; spec.get("south_pole", p) || spec.get("rotation", p)) {
        ASSERT_MSG(p.size() == 2, "Rotation: expected 'south_pole' as a list of size 2");
        lon = p[0];
        lat = p[1];
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
        custom.set("south_pole", std::vector<double>{south_pole_.lon(), south_pole_.lat()});
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
