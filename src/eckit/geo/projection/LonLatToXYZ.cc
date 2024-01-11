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

#include "eckit/geo/EllipsoidOfRevolution.h"
#include "eckit/geo/Sphere.h"
#include "eckit/geo/spec/MappedConfiguration.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static ProjectionBuilder<LonLatToXYZ> __projection("ll_to_xyz");


LonLatToXYZ::LonLatToXYZ(double a, double b) {
    ASSERT_MSG(types::is_strictly_greater(b, 0.) && b <= a, "LonLatToXYZ requires 0 < b <= a");

    struct LonLatToSphereXYZ final : Implementation {
        using S = Sphere;
        const double R_;

        explicit LonLatToSphereXYZ(double R) :
            R_(R) {}
        Point3 operator()(const PointLonLat& p) const override { return S::convertSphericalToCartesian(R_, p, 0.); }
        PointLonLat operator()(const Point3& q) const override { return S::convertCartesianToSpherical(R_, q); }
        Spec spec() const override { return Spec{{{"R", R_}}}; }
    };

    struct LonLatToSpheroidXYZ final : Implementation {
        using S = EllipsoidOfRevolution;
        const double a_;
        const double b_;

        explicit LonLatToSpheroidXYZ(double a, double b) :
            a_(a), b_(b) {}
        Point3 operator()(const PointLonLat& p) const override { return S::convertSphericalToCartesian(a_, b_, p, 0.); }
        PointLonLat operator()(const Point3& q) const override { NOTIMP; }
        Spec spec() const override { return Spec{{{"a", a_}, {"b", b_}}}; }
    };

    impl_.reset(types::is_approximately_equal(a, b) ? static_cast<Implementation*>(new LonLatToSphereXYZ(a))
                                                    : new LonLatToSpheroidXYZ(a, b));
}


LonLatToXYZ::LonLatToXYZ(double R) :
    LonLatToXYZ(R, R) {}


LonLatToXYZ::LonLatToXYZ(const Configuration& config) :
    LonLatToXYZ(config.getDouble("a", config.getDouble("R", 1.)), config.getDouble("b", config.getDouble("R", 1.))) {}


Projection::Spec LonLatToXYZ::spec() const {
    return impl_->spec();
}


}  // namespace eckit::geo::projection
