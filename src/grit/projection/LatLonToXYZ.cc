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


#include "grit/projection/LatLonToXYZ.h"

#include "grit/geometry/Sphere.h"
#include "grit/geometry/Spheroid.h"
#include "grit/util.h"


namespace grit::projection {


LatLonToXYZ::LatLonToXYZ(double a, double b) {
    ASSERT(0. < a);
    ASSERT(0. < b);

    struct LatLonToSphereXYZ final : Implementation {
        using S = geometry::Sphere;
        const double R_;

        explicit LatLonToSphereXYZ(double R) : R_(R) {}
        PointXYZ operator()(const PointLatLon& p) const override { return S::ll_to_xyz(R_, p, 0.); }
        PointLatLon operator()(const PointXYZ& q) const override { return S::xyz_to_ll(R_, q); }
    };

    struct LatLonToSpheroidXYZ final : Implementation {
        using S = geometry::Spheroid;
        const double a_;
        const double b_;

        explicit LatLonToSpheroidXYZ(double a, double b) : a_(a), b_(b) {}
        PointXYZ operator()(const PointLatLon& p) const override { return S::ll_to_xyz(a_, b_, p, 0.); }
        PointLatLon operator()(const PointXYZ& q) const override { NOTIMP; }
    };

    impl_.reset(util::is_approximately_equal(a, b) ? static_cast<Implementation*>(new LatLonToSphereXYZ(a))
                                                   : new LatLonToSpheroidXYZ(a, b));
}


LatLonToXYZ::LatLonToXYZ(double R) : LatLonToXYZ(R, R) {}


}  // namespace grit::projection
