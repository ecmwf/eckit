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


#include "eckit/geo/projection/LonLatToXYZ.h"

#include "eckit/geo/Spec.h"
#include "eckit/geo/figure/OblateSpheroid.h"
#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static const std::string TYPE("ll-to-xyz");
static ProjectionRegisterType<LonLatToXYZ> PROJECTION(TYPE);


LonLatToXYZ::LonLatToXYZ(Figure* figure_ptr) : Projection(figure_ptr) {
    struct LonLatToSphereXYZ final : Implementation {
        const double R;

        explicit LonLatToSphereXYZ(double _R) : R(_R) {}
        PointXYZ operator()(const PointLonLat& p) const override {
            return figure::Sphere::convertSphericalToCartesian(R, p, 0.);
        }
        PointLonLat operator()(const PointXYZ& q) const override {
            return figure::Sphere::convertCartesianToSpherical(R, q);
        }
    };

    struct LonLatToSpheroidXYZ final : Implementation {
        const double a;
        const double b;

        explicit LonLatToSpheroidXYZ(double _a, double _b) : a(_a), b(_b) {}
        PointXYZ operator()(const PointLonLat& p) const override {
            return figure::OblateSpheroid::convertSphericalToCartesian(a, b, p, 0.);
        }
        PointLonLat operator()(const PointXYZ& q) const override { NOTIMP; }
    };

    impl_.reset(types::is_approximately_equal(figure().eccentricity(), 0.)
                    ? static_cast<Implementation*>(new LonLatToSphereXYZ(figure().R()))
                    : new LonLatToSpheroidXYZ(figure().a(), figure().b()));
}


LonLatToXYZ::LonLatToXYZ(double R) : LonLatToXYZ(R, R) {}


LonLatToXYZ::LonLatToXYZ(double a, double b) :
    LonLatToXYZ(types::is_approximately_equal(a, b) ? static_cast<Figure*>(new geo::figure::Sphere(a))
                                                    : new geo::figure::OblateSpheroid(a, b)) {}


LonLatToXYZ::LonLatToXYZ(const Spec& spec) : LonLatToXYZ(FigureFactory::build(spec)) {}


const std::string& LonLatToXYZ::type() const {
    static const std::string type{TYPE};
    return type;
}


void LonLatToXYZ::fill_spec(spec::Custom& custom) const {
    Projection::fill_spec(custom);

    custom.set("type", TYPE);
}


}  // namespace eckit::geo::projection
