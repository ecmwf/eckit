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


#include <algorithm>
#include <array>
#include <memory>
#include <numeric>
#include <set>
#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/maths/ConvexHull.h"
#include "eckit/maths/ConvexHullN.h"
#include "eckit/testing/Test.h"


//----------------------------------------------------------------------------------------------------------------------


namespace eckit::test {


CASE("Qhull errors/exceptions") {
    using maths::ConvexHull;


    SECTION("input") {
        for (size_t N : {2, 3, 4}) {
            ConvexHull::coord_t coord((N + 1) * N + 1, 1.);
            ASSERT(coord.size() % N != 0);
            EXPECT_THROWS_AS(maths::Qhull(N, coord, "Qt"), AssertionFailed);  // 0 < N && coord.size() % N == 0
        }
    }


    SECTION("qhull") {
        struct {
            const int errorCode;
            const std::string what;
            const std::string command;
            const size_t N;
            const ConvexHull::coord_t coord;
        } static const tests[] = {
            {6050, "QH6050 qhull error: dimension 1 must be > 1", "Qt", 1, {1, 1}},
            {6154, "QH6154 Qhull precision error: Initial simplex is flat", "Qt", 2, {1, 1, 2, 2, 3, 3}},
            {6214, "QH6214 qhull input error: not enough points", "Qt", 2, {1, 1}},
            {6412, "QH6412 qhull input error (qh_initqhull_globals)", "Qt", 2, {}},
            {6421, "QH6421 qhull internal error (qh_maxsimplex)", "Qt", 2, {1, 1, 1, 1, 1, 1}},
        };

        auto trigger = [](const auto& test) { maths::Qhull(test.N, test.coord, test.command); };

        for (const auto& test : tests) {
            try {
                trigger(test);
                EXPECT(false);
            }
            catch (const ConvexHull::Exception& e) {
                EXPECT_EQUAL(test.errorCode, e.errorCode);
                EXPECT_EQUAL(test.what, std::string(e.what(), test.what.length()));
            }
            catch (...) {
                EXPECT(false);
            }
        }

        EXPECT_THROWS_AS(trigger(tests[0]), ConvexHull::DimensionError);
        EXPECT_THROWS_AS(trigger(tests[1]), ConvexHull::PrecisionError);
        EXPECT_THROWS_AS(trigger(tests[2]), ConvexHull::InputError);
        EXPECT_THROWS_AS(trigger(tests[3]), ConvexHull::InputError);

        // tests[4] does not throw a specialised error type
        try {
            trigger(tests[4]);
            EXPECT(false);
        }
        catch (const ConvexHull::DimensionError&) {
            EXPECT(false);
        }
        catch (const ConvexHull::InputError&) {
            EXPECT(false);
        }
        catch (const ConvexHull::OptionError&) {
            EXPECT(false);
        }
        catch (const ConvexHull::PrecisionError&) {
            EXPECT(false);
        }
        catch (const ConvexHull::TopologyError&) {
            EXPECT(false);
        }
        catch (const ConvexHull::Exception&) {
        }
        catch (...) {
            EXPECT(false);
        }
    }
}


CASE("ConvexHullN, N=2") {
    // Build convex hull
    maths::ConvexHullN ch(std::vector<std::array<double, 2>>{
        {1, 2},
        {3, 1},
        {4, 4},
        {6, 5},
        {7, 0},
        {2, 5},
        {5, 7},
        {8, 3},
        {6, 9},
        {9, 6},
    });

    // Inner points to convex hull
    const std::set<size_t> inner{2, 3, 6, 7 /*on edge*/};

    SECTION("vertices and facets") {
        const auto vertices = ch.list_vertices();
        EXPECT(vertices.size() == 6);

        const auto facets = ch.list_facets();
        EXPECT(facets.size() == 6);

        for (size_t vertex = 0; vertex < 10; ++vertex) {
            auto inside            = inner.find(vertex) != inner.end();
            auto count_in_vertices = std::count(vertices.begin(), vertices.end(), vertex);
            auto count_in_facets   = std::accumulate(facets.begin(), facets.end(), static_cast<size_t>(0),
                                                     [vertex](auto c, const auto& facet) {
                                                       auto f = std::count(facet.begin(), facet.end(), vertex);
                                                       EXPECT(f <= 1);
                                                       return c + f;
                                                   });
            EXPECT_EQUAL(count_in_vertices, (inside ? 0 : 1));
            EXPECT_EQUAL(count_in_facets, (inside ? 0 : 2));
        }
    }


    SECTION("factory") {
        // same as above
        std::unique_ptr<maths::ConvexHull> ch2(new maths::ConvexHullN<2>(std::vector<std::vector<double>>{
            {1, 2},
            {3, 1},
            {4, 4},
            {6, 5},
            {7, 0},
            {2, 5},
            {5, 7},
            {8, 3},
            {6, 9},
            {9, 6},
        }));

        auto vertex_set = [](const std::vector<size_t>& list) { return std::set<size_t>{list.cbegin(), list.cend()}; };
        EXPECT(vertex_set(ch.list_vertices()) == vertex_set(ch2->list_vertices()));

        // wrong dimensions
        EXPECT_THROWS_AS(new maths::ConvexHullN<3>(std::vector<std::vector<double>>{
                             {1, 2},
                             {3, 1},
                             {4, 4},
                         }),
                         AssertionFailed);
    }
}


CASE("ConvexHullN, N=3 (tetrahedron)") {
    // Hull vertices 0-3
    maths::ConvexHullN ch(std::vector<std::array<double, 3>>{
        {0, 0, 1},
        {1, 0, -1},
        {-1, 1, -1},
        {-1, -1, -1},
        {0, 0, 0},
    });


    SECTION("vertices") {
        const auto vertices = ch.list_vertices();
        EXPECT(vertices.size() == 4);

        for (size_t vertex = 0; vertex < 4; ++vertex) {
            EXPECT(std::find(vertices.begin(), vertices.end(), vertex) != vertices.end());
        }

        EXPECT(std::find(vertices.begin(), vertices.end(), 4) == vertices.end());
    }


    SECTION("facets") {
        const auto count = ch.facets_n();
        decltype(count) correct{{3, 4}};
        EXPECT(count == correct);

        auto tri = ch.facets(3);
        EXPECT(tri.size() == 4 * 3);

        const auto facets = ch.list_facets();
        for (const auto& fr : {
                 std::set<size_t>{0, 1, 2},
                 {0, 1, 3},
                 {0, 2, 3},
                 {1, 2, 3},
             }) {
            EXPECT(1 == std::count_if(facets.begin(), facets.end(), [&fr](const auto& facet) {
                       ASSERT(facet.size() == 3);
                       return std::count(fr.begin(), fr.end(), facet[0]) == 1
                              && std::count(fr.begin(), fr.end(), facet[1]) == 1
                              && std::count(fr.begin(), fr.end(), facet[2]) == 1;
                   }));

            auto count = 0;
            for (auto t = tri.begin(); t != tri.end(); t += 3) {
                count += std::set<size_t>(t, t + 3) == fr ? 1 : 0;
            }
            EXPECT_EQUAL(1, count);
        }
    }
}


CASE("ConvexHullN, N=3 (square pyramid)") {
    // Hull vertices 1-5
    maths::ConvexHullN ch(
        std::vector<std::array<double, 3>>{
            {0, 0, 0.1},
            {0, 0, 1},
            {1, 0, 0},
            {0, 1, 0},
            {-1, 0, 0},
            {0, -1, 0},
            {0, 0, 0},
            {0.1, 0.1, 0},
        },
        "Q");


    SECTION("vertices") {
        const auto vertices = ch.list_vertices();

        const std::set<size_t> result(vertices.begin(), vertices.end());
        decltype(result) correct{1, 2, 3, 4, 5};
        EXPECT(result == correct);
    }


    SECTION("facets") {
        // test facets_n(), facets(size_t)
        const std::vector<std::set<size_t>> correct{{1, 2, 3}, {1, 3, 4}, {1, 4, 5}, {1, 5, 2}, {2, 3, 4, 5}};

        auto counts = ch.facets_n();
        EXPECT(4 == counts[3]);
        EXPECT(1 == counts[4]);
        EXPECT(5 == std::accumulate(counts.begin(), counts.end(), 0, [](size_t sum, const auto& count) {
                   return sum + count.second;
               }));

        counts[5] = 0;  // test non-existant 5-vertex facets

        for (const auto [Nv, Nf] : counts) {
            const auto facets = ch.facets(Nv);
            EXPECT(facets.size() == Nv * Nf);

            auto df = static_cast<decltype(facets)::difference_type>(Nv);

            for (auto f = facets.begin(); f != facets.end(); f += df) {
                const std::set<size_t> result(f, f + df);
                EXPECT(1 == std::count_if(correct.begin(), correct.end(), [&result](const auto& correct) {
                           return result == correct;
                       }));
            }
        }


        // test list_facets()
        const auto facets = ch.list_facets();
        EXPECT(facets.size() == correct.size());

        for (const auto& facet : facets) {
            const std::set<size_t> result(facet.begin(), facet.end());
            EXPECT(1 == std::count_if(correct.begin(), correct.end(), [&result](const auto& correct) {
                       return result == correct;
                   }));
        }
    }
}


CASE("Triangulation, N=3") {
    auto tri = maths::Qhull(3,
                            {
                                0.1, 0.1, 0.1,  //
                                0, 0, 0,        //
                                1, 0, 0,        //
                                0, 1, 0,        //
                                0, 0, 1,        //
                            },
                            "Qt")
                   .facets(3);

    EXPECT_EQUAL(tri.size(), 4 * 3);

    auto find_triangle = [&tri](const std::set<size_t>& fr) {
        auto count = 0;
        for (auto t = tri.begin(); t != tri.end(); t += 3) {
            count += std::set<size_t>(t, t + 3) == fr ? 1 : 0;
        }
        return 1 == count;
    };

    EXPECT(find_triangle({1, 2, 3}));
    EXPECT(find_triangle({1, 2, 4}));
    EXPECT(find_triangle({1, 3, 4}));
    EXPECT(find_triangle({2, 3, 4}));
}


}  // namespace eckit::test


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
