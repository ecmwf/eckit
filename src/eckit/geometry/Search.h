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


#pragma once

#include "eckit/container/KDTree.h"
#include "eckit/container/sptree/SPValue.h"
#include "eckit/geometry/Point2.h"
#include "eckit/geometry/Point3.h"
#include "eckit/geometry/PointLonLat.h"
#include "eckit/geometry/UnitSphere.h"


namespace eckit::geometry {


namespace search {
template <typename PointT, typename PayloadT>
struct Traits {
    using Point   = PointT;
    using Payload = PayloadT;
};
}  // namespace search


using Search3 = KDTreeMemory<search::Traits<Point3, size_t>>;


using Search2 = KDTreeMemory<search::Traits<Point2, size_t>>;


struct SearchLonLat : Search3 {
    using Point = PointLonLat;
    using Value = SPValue<TT<search::Traits<Point, Payload>, KDMemory>>;

    using Search3::Search3;

    void insert(const SearchLonLat::Value& value) {
        Search3::insert({convert(value.point()), value.payload()});
    }

    template <typename ITER>
    void build(ITER begin, ITER end) {
        for (auto it = begin; it != end; ++it) {
            insert(*it);
        }
    }

    template <typename Container>
    void build(Container& c) {
        build(c.begin(), c.end());
    }

    NodeInfo nearestNeighbour(const Point& p) { return Search3::nearestNeighbour(convert(p)); }

    NodeList findInSphere(const Point& p, double radius) {
        return Search3::findInSphere(convert(p), radius);
    }

    NodeList kNearestNeighbours(const Point& p, size_t k) {
        return Search3::kNearestNeighbours(convert(p), k);
    }

private:
    static Search3::Point convert(const Point& p) {
        return UnitSphere::convertSphericalToCartesian(p);
    }
};


}  // namespace eckit::geometry
