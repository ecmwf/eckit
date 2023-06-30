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


#include "eckit/grid/projection/LonLatToXYZ.h"

#include "eckit/config/Configuration.h"
#include "eckit/geometry/EllipsoidOfRevolution.h"
#include "eckit/geometry/Sphere.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::grid::projection {


static ProjectionBuilder<LonLatToXYZ> __projection("ll_to_xyz");


LonLatToXYZ::LonLatToXYZ(double a, double b) {
    ASSERT(0. < a);
    ASSERT(0. < b);

    struct LonLatToSphereXYZ final : Implementation {
        using S = geometry::Sphere;
        const double R_;

        explicit LonLatToSphereXYZ(double R) :
            R_(R) {}
        Point3 operator()(const PointLonLat& p) const override {
            return S::convertSphericalToCartesian(R_, p, 0.);
        }
        PointLonLat operator()(const Point3& q) const override {
            return S::convertCartesianToSpherical(R_, q);
        }
    };

    struct LonLatToSpheroidXYZ final : Implementation {
        using S = geometry::EllipsoidOfRevolution;
        const double a_;
        const double b_;

        explicit LonLatToSpheroidXYZ(double a, double b) :
            a_(a), b_(b) {}
        Point3 operator()(const PointLonLat& p) const override {
            return S::convertSphericalToCartesian(a_, b_, p, 0.);
        }
        PointLonLat operator()(const Point3& q) const override { NOTIMP; }
    };

    impl_.reset(types::is_approximately_equal(a, b) ? static_cast<Implementation*>(new LonLatToSphereXYZ(a))
                                                    : new LonLatToSpheroidXYZ(a, b));
}


LonLatToXYZ::LonLatToXYZ(double R) :
    LonLatToXYZ(R, R) {}


LonLatToXYZ::LonLatToXYZ(const Configuration& config) :
    LonLatToXYZ(config.getDouble("a", config.getDouble("R", 1.)),
                config.getDouble("b", config.getDouble("R", 1.))) {}


}  // namespace eckit::grid::projection
