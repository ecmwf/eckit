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

#include <tuple>
#include <vector>

#include "eckit/container/KDTree.h"
#include "eckit/container/sptree/SPValue.h"
#include "eckit/geometry/Point.h"
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

    using Result  = std::tuple<Point, Payload, double>;
    using Results = std::vector<Result>;

    using Search3::Search3;

    void insert(const SearchLonLat::Value& value) {
        Search3::insert({to_cartesian(value.point()), value.payload()});
    }

    template <typename Container>
    void build(const Container& c) {
        size_t index = 0;
        for (const auto& p : c) {
            insert({p, index++});
        }
    }

    Result nearestNeighbour(const Point& p) {
        auto n = Search3::nearestNeighbour(to_cartesian(p));
        return {UnitSphere::convertCartesianToSpherical(n.point()), n.payload(), n.distance()};
    }

    Results findInSphere(const Point& p, double radius) {
        return to_spherical(Search3::findInSphere(to_cartesian(p), radius));
    }

    Results kNearestNeighbours(const Point& p, size_t k) {
        return to_spherical(Search3::kNearestNeighbours(to_cartesian(p), k));
    }

private:
    static Search3::Point to_cartesian(const Point& p) {
        return UnitSphere::convertSphericalToCartesian(p);
    }

    static Results to_spherical(const NodeList& nodes) {
        Results list;
        list.reserve(nodes.size());
        for (const auto& n : nodes) {
            list.emplace_back(UnitSphere::convertCartesianToSpherical(n.point()), n.payload(), n.distance());
        }
        return list;
    }
};


}  // namespace eckit::geometry
