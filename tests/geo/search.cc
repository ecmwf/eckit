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


#include <vector>

#include "eckit/container/KDTree.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/PointXY.h"
#include "eckit/geo/PointXYZ.h"
#include "eckit/geo/Search.h"
#include "eckit/geo/projection/LonLatToXYZ.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


template <typename PointT, typename PayloadT>
struct Traits {
    using Point   = PointT;
    using Payload = PayloadT;
};


CASE("SearchXY (non-cacheable)") {
    using SearchXY = KDTreeMemory<Traits<PointXY, size_t>>;

    std::vector<SearchXY::Value> points{
        {{0., 0.}, 0},
    };

    SearchXY search;
    search.build(points);

    auto a = search.nearestNeighbour({0.1, 0.});
    EXPECT_EQUAL(a.payload(), 0);

    auto b = search.nearestNeighbour({0.9, 0.});
    EXPECT_EQUAL(b.payload(), 0);

    search.insert({{1., 0.}, 1});

    auto c = search.nearestNeighbour({0.1, 0.});
    EXPECT_EQUAL(c.payload(), 0);

    auto d = search.nearestNeighbour({0.9, 0.});
    EXPECT_EQUAL(d.payload(), 1);
}


CASE("SearchXYZ (non-cacheable)") {
    using SearchXYZ = KDTreeMemory<Traits<PointXYZ, size_t>>;

    std::vector<SearchXYZ::Value> points{
        {{0., 0., 0.}, 0},
    };

    SearchXYZ search;
    search.build(points);

    auto a = search.nearestNeighbour({0.1, 0., 0.});
    EXPECT_EQUAL(a.payload(), 0);

    auto b = search.nearestNeighbour({0.9, 0., 0.});
    EXPECT_EQUAL(b.payload(), 0);

    search.insert({{1., 0., 0.}, 1});

    auto c = search.nearestNeighbour({0.1, 0., 0.});
    EXPECT_EQUAL(c.payload(), 0);

    auto d = search.nearestNeighbour({0.9, 0., 0.});
    EXPECT_EQUAL(d.payload(), 1);
}


CASE("Search (cacheable)") {
    std::unique_ptr<const Grid> grid(GridFactory::build(spec::Custom{{{"grid", "30/30"}}}));

    const Search search(*grid);
    std::vector<Search::PointValueType> closest;

    projection::LonLatToXYZ to_xyz;
    Search::ValueType index = 0;

    auto points_equal = [](const auto& p, const auto& q) {
        return geo::points_equal(PointXYZ{p.x(0), p.x(1), p.x(2)}, PointXYZ{q.x(0), q.x(1), q.x(2)});
    };

    for (const auto& p : *grid) {
        Search::PointType q(to_xyz.fwd(std::get<PointLonLat>(p)));

        search.closestNPoints(q, 2, closest);
        EXPECT(closest.size() == 2);

        EXPECT(points_equal(closest.front().point(), q));

        if (!std::get<PointLonLat>(p).pole()) {
            EXPECT(closest.front().payload() == index);
        }

        ++index;
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
