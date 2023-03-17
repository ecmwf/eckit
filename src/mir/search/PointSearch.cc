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
#include "eckit/thread/AutoLock.h"

#include "mir/config/LibMir.h"
#include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/Representation.h"
#include "mir/util/Log.h"
#include "mir/util/Trace.h"
#include "mir/util/Types.h"


namespace mir::search {


static std::string extract_loader(const param::MIRParametrisation& param) {
    bool caching = LibMir::caching();
    param.get("caching", caching);

    static const std::string cacheable =
        eckit::Resource<std::string>("$MIR_POINT_SEARCH_LOADER;mirPointSearchLoader", "mapped-cache-file");

    std::string name = caching ? cacheable : "memory";
    param.get("point-search-trees", name);
    return name;
}


PointSearch::PointSearch(const param::MIRParametrisation& param, const repres::Representation& r) {
    tree_.reset(TreeFactory::build(extract_loader(param), r));
    eckit::AutoLock<Tree> lock(*tree_);

    Log::debug() << "Search using " << *tree_ << std::endl;

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

    trace::Timer timer("PointSearch: building k-d tree");
    Log::info() << "PointSearch: building " << *tree_ << " for " << r << " (" << Log::Pretty(npts, {"point"}) << ")"
                << std::endl;

    static bool fastBuildKDTrees =
        eckit::Resource<bool>("$ATLAS_FAST_BUILD_KDTREES", true);  // We use the same Resource as ATLAS for now

    if (fastBuildKDTrees) {
        std::vector<PointValueType> points;
        points.reserve(npts);

        for (const std::unique_ptr<repres::Iterator> it(r.iterator()); it->next();) {
            points.emplace_back(PointValueType(it->point3D(), it->index()));
        }

        tree_->build(points);

        if (points.size() < npts) {
            Log::info() << "PointSearch: built tree for " << Log::Pretty(points.size(), {"valid point"}) << std::endl;
        }
    }
    else {
        for (const std::unique_ptr<repres::Iterator> it(r.iterator()); it->next();) {
            tree_->insert(PointValueType(it->point3D(), it->index()));
        }
    }
}


void PointSearch::print(std::ostream& out) const {
    tree_->statsPrint(out, false);
    tree_->statsReset();
}


}  // namespace mir::search
