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

#include <vector>

#include "eckit/container/KDTree.h"
#include "eckit/container/sptree/SPValue.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/figure/UnitSphere.h"


namespace eckit::geo {


namespace search {
template <typename PointT, typename PayloadT>
struct Traits {
    using Point   = PointT;
    using Payload = PayloadT;
};
}  // namespace search


using Search3 = KDTreeMemory<search::Traits<geo::PointXYZ, size_t>>;


using Search2 = KDTreeMemory<search::Traits<geo::PointXY, size_t>>;


struct SearchLonLat : Search3 {
    using Point = geo::PointLonLat;
    using Value = SPValue<TT<search::Traits<Point, Payload>, KDMemory>>;

    using Search3::Search3;

    void insert(const SearchLonLat::Value& value) { Search3::insert({to_cartesian(value.point()), value.payload()}); }

    template <typename Container>
    void build(const Container& c) {
        size_t index = 0;
        for (const auto& p : c) {
            insert({std::get<Point>(p), index++});
        }
    }

    size_t nearestNeighbour(const Point& p) {
        auto n = Search3::nearestNeighbour(to_cartesian(p));
        return n.payload();
    }

    std::vector<size_t> findInSphere(const Point& p, double radius) {
        std::vector<size_t> near;
        for (auto& n : Search3::findInSphere(to_cartesian(p), radius)) {
            near.emplace_back(n.payload());
        }
        return near;
    }

    std::vector<size_t> kNearestNeighbours(const Point& p, size_t k) {
        std::vector<size_t> near;
        for (auto& n : Search3::kNearestNeighbours(to_cartesian(p), k)) {
            near.emplace_back(n.payload());
        }
        return near;
    }

private:

    static Search3::Point to_cartesian(const Point& p) { return figure::UnitSphere::_convertSphericalToCartesian(p); }
};


}  // namespace eckit::geo
