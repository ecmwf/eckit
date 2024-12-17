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
#include "eckit/geo/geometry/OblateSpheroid.h"
#include "eckit/geo/geometry/Sphere.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static ProjectionBuilder<LonLatToXYZ> PROJECTION("ll_to_xyz");


LonLatToXYZ::LonLatToXYZ(Figure* figure_ptr) : ProjectionOnFigure(figure_ptr) {
    struct LonLatToSphereXYZ final : Implementation {
        const double R;

        explicit LonLatToSphereXYZ(double _R) : R(_R) {}
        Point3 operator()(const PointLonLat& p) const override {
            return geometry::Sphere::convertSphericalToCartesian(R, p, 0.);
        }
        PointLonLat operator()(const Point3& q) const override {
            return geometry::Sphere::convertCartesianToSpherical(R, q);
        }
    };

    struct LonLatToSpheroidXYZ final : Implementation {
        const double a;
        const double b;

        explicit LonLatToSpheroidXYZ(double _a, double _b) : a(_a), b(_b) {}
        Point3 operator()(const PointLonLat& p) const override {
            return geometry::OblateSpheroid::convertSphericalToCartesian(a, b, p, 0.);
        }
        PointLonLat operator()(const Point3& q) const override { NOTIMP; }
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


void LonLatToXYZ::fill_spec(spec::Custom& custom) const {
    ProjectionOnFigure::fill_spec(custom);

    custom.set("projection", "ll_to_xyz");
}


}  // namespace eckit::geo::projection
