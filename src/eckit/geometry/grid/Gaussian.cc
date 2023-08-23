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


#include "eckit/geometry/grid/Gaussian.h"

#include <algorithm>
#include <cmath>
#include <map>

#include "eckit/config/Configuration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/geometry/Domain.h"
#include "eckit/geometry/util.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geometry::grid {


Gaussian::Gaussian(size_t N, const area::BoundingBox& bbox) :
    Grid(bbox), N_(N), angularPrecision_(0) {
    ASSERT(N_ > 0);
}


Gaussian::Gaussian(const Configuration& config) :
    Grid(config), N_(config.getUnsigned("N")), angularPrecision_(config.getDouble("angular_precision", 0)) {
    ASSERT(N_ > 0);
    ASSERT(angularPrecision_ >= 0);
}


const std::vector<double>& Gaussian::latitudes() const {
    return latitudes_.empty() ? (latitudes_ = util::gaussian_latitudes(N_, false)) : latitudes_;
}


Gaussian::~Gaussian() = default;


bool Gaussian::includesNorthPole() const {
    return bbox().north() >= latitudes().front();
}


bool Gaussian::includesSouthPole() const {
    return bbox().south() <= latitudes().back();
}


bool Gaussian::angleApproximatelyEqual(const double& A, const double& B) const {
    return angularPrecision_ > 0 ? types::is_approximately_equal(A, B, angularPrecision_)
                                 : A == B;
}


void Gaussian::correctSouthNorth(double& s, double& n, bool in) const {
    ASSERT(s <= n);

    const std::vector<double>& lats = latitudes();
    ASSERT(!lats.empty());

    const bool same(s == n);
    if (n < lats.back()) {
        n = lats.back();
    }
    else if (in) {
        auto best = std::lower_bound(lats.begin(), lats.end(), n, [this](double l1, double l2) {
            if (angleApproximatelyEqual(l1, l2)) {
                return false;
            }
            return !(l1 < l2);
        });
        ASSERT(best != lats.end());
        n = *best;
    }
    else if (n > lats.front()) {
        // extend 'outwards': don't change, it's already above the Gaussian latitudes
    }
    else {
        auto best = std::lower_bound(lats.rbegin(), lats.rend(), n);
        n         = *best;
    }

    if (same && in) {
        s = n;
    }
    else if (s > lats.front()) {
        s = lats.front();
    }
    else if (in) {
        auto best = std::lower_bound(lats.rbegin(), lats.rend(), s, [this](double l1, double l2) {
            if (angleApproximatelyEqual(l1, l2)) {
                return false;
            }
            return !(l1 > l2);
        });
        ASSERT(best != lats.rend());
        s = *best;
    }
    else if (s < lats.back()) {
        // extend 'outwards': don't change, it's already below the Gaussian latitudes
    }
    else {
        auto best = std::lower_bound(lats.begin(), lats.end(), s, [](double l1, double l2) { return l1 > l2; });
        s         = *best;
    }

    ASSERT(s <= n);
}


}  // namespace eckit::geometry::grid
