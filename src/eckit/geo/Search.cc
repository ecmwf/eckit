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


#include "eckit/geo/Search.h"

#include "eckit/config/Resource.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Iterator.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/Trace.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/log/ResourceUsage.h"
#include "eckit/thread/AutoLock.h"


namespace eckit::geo {


static std::string extract_loader(const spec::Spec& spec) {
    static const std::string default_loader =
        spec.get_bool("caching", LibEcKitGeo::caching())
            ? std::string{LibResource<std::string, LibEcKitGeo>("$ECKIT_GEO_SEARCH_LOADER;ecKitGeoSearchLoader",
                                                                "mapped-cache-file")}
            : "memory";

    return spec.get_string("eckit-geo-search-trees", default_loader);
}


Search::Search(const Grid& r, const spec::Spec& spec) {
    ResourceUsage usage("Search: build tree");

    tree_.reset(search::TreeFactory::build(extract_loader(spec), r));
    AutoLock<search::Tree> lock(*tree_);

    if (!tree_->ready()) {
        build(r);
        tree_->commit();
    }
}


Search::PointValueType Search::closestPoint(const Search::PointType& pt) const {
    return tree_->nearestNeighbour(pt);
}


void Search::closestNPoints(const PointType& pt, size_t n, std::vector<PointValueType>& closest) const {
    // Small optimisation
    if (n == 1) {
        closest.clear();
        closest.push_back(closestPoint(pt));
        return;
    }

    closest = tree_->kNearestNeighbours(pt, n);
}


void Search::closestWithinRadius(const PointType& pt, double radius, std::vector<PointValueType>& closest) const {
    closest = tree_->findInSphere(pt, radius);
}


void Search::build(const Grid& r) {
    Trace trace("eckit::geo::Search: building k-d tree");

    const auto npts = r.size();
    ASSERT(npts == tree_->itemCount());

    // assumes grid describes lat/lon
    projection::LonLatToXYZ to_xyz;
    size_t index = 0;

    if (static bool fast = Resource<bool>("$ECKIT_GEO_SEARCH_FAST_BUILD", true); fast) {
        std::vector<PointValueType> points;
        points.reserve(npts);

        for (const auto& p : r) {
            auto q = to_xyz.fwd(std::get<PointLonLat>(p));
            points.emplace_back(Search::PointType{q.X(), q.Y(), q.Z()}, index++);
        }

        tree_->build(points);
        return;
    }

    for (const auto& p : r) {
        auto q = to_xyz.fwd(std::get<PointLonLat>(p));
        tree_->insert({Search::PointType{q.X(), q.Y(), q.Z()}, index++});
    }
}


void Search::print(std::ostream& out) const {
    tree_->statsPrint(out, false);
    tree_->statsReset();
}


}  // namespace eckit::geo
