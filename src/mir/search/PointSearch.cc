/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/search/PointSearch.h"

#include "eckit/config/Resource.h"
#include "eckit/log/Plural.h"
#include "eckit/log/Timer.h"

#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/Representation.h"


namespace mir {
namespace search {


PointSearch::PointSearch(const param::MIRParametrisation& parametrisation, const repres::Representation& r) :
    parametrisation_(parametrisation) {
    const size_t npts = r.numberOfPoints();
    ASSERT(npts > 0);

    std::string name = "tree-mapped-cache-file";
    parametrisation.get("point-search", name);

    tree_.reset(TreeFactory::build(name, r, parametrisation, npts));

    eckit::AutoLock<Tree> lock(*tree_);

    eckit::Log::debug<LibMir>() << "Search using " << *tree_ << std::endl;

    if (!tree_->ready()) {
        build(r);
        tree_->commit();
    }
}


void PointSearch::build(const repres::Representation& r) {
    const size_t npts = r.numberOfPoints();

    eckit::Timer timer("Building KDTree");
    eckit::Log::info() << "Building " << *tree_ << " for " << r << " (" << eckit::Plural(npts, "point") << ")"
                       << std::endl;

    static bool fastBuildKDTrees =
        eckit::Resource<bool>("$ATLAS_FAST_BUILD_KDTREES", true); // We use the same Resource as ATLAS for now

    if (fastBuildKDTrees) {
        std::vector<PointValueType> points;
        points.reserve(npts);

        const eckit::ScopedPtr<repres::Iterator> it(r.iterator());
        size_t i = 0;
        while (it->next()) {
            ASSERT(i < npts);
            points.emplace_back(PointValueType(it->point3D(), i));
            ++i;
        }

        tree_->build(points);
    } else {
        const eckit::ScopedPtr<repres::Iterator> it(r.iterator());
        size_t i = 0;
        while (it->next()) {
            ASSERT(i < npts);
            tree_->insert(PointValueType(it->point3D(), i));
            ++i;
        }
    }
}


void PointSearch::statsPrint(std::ostream& out, bool fancy) const {
    tree_->statsPrint(out, fancy);
}


void PointSearch::statsReset() const {
    tree_->statsReset();
}


PointSearch::PointValueType PointSearch::closestPoint(const PointSearch::PointType& pt) const {
    return tree_->nearestNeighbour(pt);
}


void PointSearch::closestNPoints(const PointType& pt, size_t n, std::vector<PointValueType>& closest) const {

    // Small optimisation
    if (n == 1) {
        closest.clear();
        closest.push_back(closestPoint(pt));
        return;
    }

    closest = tree_->kNearestNeighbours(pt, n);
}


void PointSearch::closestWithinRadius(const PointType& pt, double radius, std::vector<PointValueType>& closest) const {
    closest = tree_->findInSphere(pt, radius);
}


} // namespace search
} // namespace mir
