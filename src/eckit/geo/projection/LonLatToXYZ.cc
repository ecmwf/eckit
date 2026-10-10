// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/LonLatToXYZ.h"

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/figure/OblateSpheroid.h"
#include "eckit/geo/figure/Sphere.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static const std::string TYPE("ll-to-xyz");
static ProjectionRegisterType<LonLatToXYZ> PROJECTION(TYPE);


LonLatToXYZ::LonLatToXYZ(Figure* figure_ptr) :
    Projection(figure_ptr, PointLonLat{}, PointXYZ{}),
    a_(figure().a()),
    b_(figure().b()),
    spherical_(figure().spherical()) {}


LonLatToXYZ::LonLatToXYZ(double R) : LonLatToXYZ(R, R) {}


LonLatToXYZ::LonLatToXYZ(double a, double b) :
    LonLatToXYZ(types::is_approximately_equal(a, b) ? static_cast<Figure*>(new geo::figure::Sphere(a))
                                                    : new geo::figure::OblateSpheroid(a, b)) {}


LonLatToXYZ::LonLatToXYZ(const Spec& spec) : LonLatToXYZ(FigureFactory::build(spec)) {}


PointXYZ LonLatToXYZ::fwd(const PointLonLat& p) const {
    return spherical_ ? figure::Sphere::convertSphericalToCartesian(a_, p)
                      : figure::OblateSpheroid::convertSphericalToCartesian(a_, b_, p);
}


PointLonLat LonLatToXYZ::inv(const PointXYZ& q) const {
    if (!spherical_) {
        NOTIMP;
    }

    return figure::Sphere::convertCartesianToSpherical(a_, q);
}


const std::string& LonLatToXYZ::type() const {
    static const std::string type{TYPE};
    return type;
}


void LonLatToXYZ::fill_spec(spec::Custom& custom) const {
    Projection::fill_spec(custom);

    custom.set("type", TYPE);
}


std::vector<std::vector<double>> LonLatToXYZ::fwd_vector(const std::vector<double>& lon, const std::vector<double>& lat,
                                                         const std::vector<double>&) const {
    auto convert = [&lon, &lat](const auto& to_xyz) {
        std::vector<std::vector<double>> xyz(3, std::vector<double>(lon.size()));
        for (size_t i = 0; i < lon.size(); ++i) {
            const auto q = to_xyz(PointLonLat{lon[i], lat[i]});
            xyz[0][i]    = q.X();
            xyz[1][i]    = q.Y();
            xyz[2][i]    = q.Z();
        }
        return xyz;
    };

    if (spherical_) {
        return convert([R = a_](const PointLonLat& p) { return figure::Sphere::convertSphericalToCartesian(R, p); });
    }

    return convert([a = a_, b = b_](const PointLonLat& p) {
        return figure::OblateSpheroid::convertSphericalToCartesian(a, b, p);
    });
}


}  // namespace eckit::geo::projection
