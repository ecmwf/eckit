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


#include "mir/search/PointSearch.h"

#include "eckit/config/Resource.h"
#include "eckit/log/Timer.h"
#include "eckit/thread/AutoLock.h"

#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/Representation.h"
#include "mir/util/Pretty.h"


namespace mir {
namespace search {

PointSearch::PointSearch(const param::MIRParametrisation& parametrisation, const repres::Representation& r) {

    std::string name = "mapped-cache-file";
    parametrisation.get("point-search-trees", name);

    tree_.reset(TreeFactory::build(name, r, parametrisation));

    eckit::AutoLock<Tree> lock(*tree_);

    eckit::Log::debug<LibMir>() << "Search using " << *tree_ << std::endl;

    if (!tree_->ready()) {
        build(r);
        tree_->commit();
    }
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

void PointSearch::build(const repres::Representation& r) {
    const size_t npts = tree_->itemCount();
    ASSERT(npts > 0);

    eckit::Timer timer("PointSearch: building k-d tree");
    eckit::Log::info() << "PointSearch: building " << *tree_ << " for " << r << " (" << Pretty(npts, {"point"}) << ")"
                       << std::endl;

    static bool fastBuildKDTrees =
        eckit::Resource<bool>("$ATLAS_FAST_BUILD_KDTREES", true);  // We use the same Resource as ATLAS for now

    if (fastBuildKDTrees) {
        std::vector<PointValueType> points;
        points.reserve(npts);

        const std::unique_ptr<repres::Iterator> it(r.iterator());
        size_t i = 0;
        while (it->next()) {
            ASSERT(i < npts);
            points.emplace_back(PointValueType(it->point3D(), i));
            ++i;
        }

        ASSERT(npts == i);
        tree_->build(points);
    }
    else {
        const std::unique_ptr<repres::Iterator> it(r.iterator());
        size_t i = 0;
        while (it->next()) {
            ASSERT(i < npts);
            tree_->insert(PointValueType(it->point3D(), i));
            ++i;
        }

        ASSERT(npts == i);
    }
}

void PointSearch::print(std::ostream& out) const {
    tree_->statsPrint(out, false);
    tree_->statsReset();
}

}  // namespace search
}  // namespace mir
