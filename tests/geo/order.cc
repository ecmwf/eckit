// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <algorithm>
#include <cstddef>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/order/HEALPix.h"
#include "eckit/geo/order/ReducedScan.h"
#include "eckit/geo/order/RegularScan.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("order=scan") {
    using order::RegularScan;

    static const std::map<std::string, long> modes{
        {"i+j+", 64},    //
        {"i+j-", 0},     //
        {"i-j+", 192},   //
        {"i-j-", 128},   //
        {"i-+j+", 208},  //
        {"i-+j-", 144},  //
        {"i+-j+", 80},   //
        {"i+-j-", 16},   //
        {"j+i+", 96},    //
        {"j+i-", 224},   //
        {"j-i+", 32},    //
        {"j-i-", 160},   //
        {"j-+i+", 32},   //
        {"j-+i-", 160},  //
        {"j+-i+", 96},   //
        {"j+-i-", 224},  //
    };

    for (const auto& [order, scanningMode] : modes) {
        const RegularScan scan(order);
        EXPECT(scan.is_scan_i_then_j() == (order.front() == 'i'));
        EXPECT(scan.is_scan_i_positive() == (order.find("i+") != std::string::npos));
        EXPECT(scan.is_scan_j_positive() == (order.find("j+") != std::string::npos));

        bool alternating = order.find("+-") != std::string::npos || order.find("-+") != std::string::npos;
        EXPECT(scan.is_scan_alternating() == alternating);

        EXPECT(order == scan.order());


        auto alternativeRowScanning = order.find("i+-") != std::string::npos || order.find("i-+") != std::string::npos;
        auto jPointsAreConsecutive  = order.front() == 'j';
        auto jScansPositively       = order.find("j+") != std::string::npos;
        auto iScansNegatively       = order.find("i-") != std::string::npos;

        long mode = (alternativeRowScanning ? (1 << 4) : 0) |  //
                    (jPointsAreConsecutive ? (1 << 5) : 0) |   //
                    (jScansPositively ? (1 << 6) : 0) |        //
                    (iScansNegatively ? (1 << 7) : 0);
        EXPECT(mode == scanningMode);
    }

    // the empty order is the default
    EXPECT(RegularScan{""}.order() == RegularScan::order_default());
    EXPECT(RegularScan{""}.order() == "i+j-");
}


CASE("order=scan, reorder") {
    using order::RegularScan;
    constexpr size_t ni = 3;
    constexpr size_t nj = 2;

    // points, by their index in the default order (i+j-)
    const std::map<std::string, RegularScan::renumber_type> renumbered{
        {"i+j-", {0, 1, 2, 3, 4, 5}},  {"i+j+", {3, 4, 5, 0, 1, 2}}, {"i-j-", {2, 1, 0, 5, 4, 3}},
        {"i-j+", {5, 4, 3, 2, 1, 0}},  {"j-i+", {0, 3, 1, 4, 2, 5}}, {"j+i+", {3, 0, 4, 1, 5, 2}},
        {"j-i-", {2, 5, 1, 4, 0, 3}},  {"j+i-", {5, 2, 4, 1, 3, 0}}, {"i+-j-", {0, 1, 2, 5, 4, 3}},
        {"j-+i+", {0, 3, 4, 1, 2, 5}},
    };
    for (const auto& r : renumbered) {
        EXPECT(RegularScan{r.first}.reorder(RegularScan::order_default(), ni, nj) == r.second);
    }

    const std::vector<std::string> orders{"i+j+", "i+j-", "i-j+", "i-j-", "i-+j+", "i-+j-", "i+-j+", "i+-j-",
                                          "j+i+", "j+i-", "j-i+", "j-i-", "j-+i+", "j-+i-", "j+-i+", "j+-i-"};

    for (const auto& a : orders) {
        for (const auto& b : orders) {
            const auto ab = RegularScan{a}.reorder(b, ni, nj);
            const auto ba = RegularScan{b}.reorder(a, ni, nj);

            for (size_t k = 0; k < ab.size(); ++k) {
                EXPECT(ba[ab[k]] == k);
            }
        }
    }
}


CASE("healpix") {
    using order::HEALPix;

    SECTION("order=ring/nested") {
        size_t Nside = 4;

        EXPECT(HEALPix::order_default() == HEALPix(spec::Custom{{"Nside", Nside}}).order());
        EXPECT(HEALPix::RING == HEALPix(spec::Custom{{"Nside", Nside}, {"order", HEALPix::RING}}).order());
        EXPECT(HEALPix::NESTED == HEALPix(spec::Custom{{"Nside", Nside}, {"order", HEALPix::NESTED}}).order());

        for (const auto& type : {HEALPix::order_default(), HEALPix::RING, HEALPix::NESTED}) {
            HEALPix order(Nside, type);
            auto no_reorder = order.reorder(type);

            std::vector<size_t> expected(no_reorder.size());
            std::iota(expected.begin(), expected.end(), 0);
            EXPECT(expected == no_reorder);
        }
    }


    SECTION("nested to row (ring) and position") {
        for (size_t Nside : {1, 2, 4, 8}) {
            const HEALPix::Renumber renumber(Nside);

            // points before each ring (rings of 4, 8, ... 4 Nside, ... 8, 4 points)
            std::vector<size_t> acc{0};
            for (size_t j = 0; j < 4 * Nside - 1; ++j) {
                const auto k = std::min(j + 1, 4 * Nside - 1 - j);
                acc.push_back(acc.back() + 4 * std::min(k, Nside));
            }

            for (size_t n = 0; n < acc.back(); ++n) {
                const auto row_i = renumber.nest_to_row(n);
                const auto row   = row_i.first;
                const auto i     = row_i.second;
                EXPECT(row + 1 < acc.size() && i < acc[row + 1] - acc[row]);
                EXPECT_EQUAL(acc[row] + i, renumber.nest_to_ring(n));
                EXPECT_EQUAL(renumber.ring_to_nest(renumber.nest_to_ring(n)), n);
            }
        }
    }


    SECTION("exceptions") {
        EXPECT_THROWS_AS(HEALPix(spec::Custom{{"Nside", 2}, {"order", "?"}}), exception::OrderError);

        EXPECT_THROWS_AS(HEALPix(2, "?"), exception::OrderError);
        EXPECT_THROWS_AS(HEALPix(2).reorder("?"), exception::OrderError);

        // nested: Nside a power of 2, up to 2^13 (checked before any int arithmetic)
        const std::vector<size_t> Nsides{size_t{0}, size_t{3}, size_t{1} << 14, size_t{1} << 40};
        for (size_t Nside : Nsides) {
            EXPECT_THROWS_AS(HEALPix::Renumber{Nside}, exception::OrderError);
        }
        EXPECT_NO_THROW(HEALPix::Renumber{size_t{1} << 13});

        for (size_t Nside : {1, 2, 3}) {
            EXPECT_NO_THROW(HEALPix{Nside});
            EXPECT_NO_THROW(HEALPix(Nside, HEALPix::order_default()));
        }

        // nested: Nside a power of 2, also to reorder to
        EXPECT_THROWS_AS(HEALPix(3, HEALPix::NESTED), exception::OrderError);
        EXPECT_THROWS_AS(HEALPix(3).reorder(HEALPix::NESTED), exception::OrderError);
    }
}


CASE("order=reduced scan") {
    using order::ReducedScan;

    EXPECT(ReducedScan{}.order() == ReducedScan::order_default());
    EXPECT(ReducedScan{""}.order() == "i+j-");

    // rows of points: consecutive along rows, rows not alternating direction
    const std::vector<std::string> supported{"i+j-", "i+j+", "i-j-", "i-j+"};
    for (const std::string& order : supported) {
        EXPECT_NO_THROW(ReducedScan{order});
    }

    const std::vector<std::string> unsupported{"j+i+", "j-i-", "i+-j-", "i-+j+", "j+-i+", "ring", "x"};
    for (const std::string& order : unsupported) {
        EXPECT_THROWS_AS(ReducedScan{order}, exception::OrderError);
    }

    // rows of 2, 0, 3 and 1 points (rows in the default order, north to south), by the points before each row then
    // the number of points: index in the default order of each point
    const std::vector<size_t> nxacc{0, 2, 2, 5, 6};

    const std::map<std::string, ReducedScan::renumber_type> expected{
        {"i+j-", {0, 1, 2, 3, 4, 5}},
        {"i+j+", {5, 2, 3, 4, 0, 1}},
        {"i-j-", {1, 0, 4, 3, 2, 5}},
        {"i-j+", {5, 4, 3, 2, 1, 0}},
    };

    for (const auto& e : expected) {
        const ReducedScan scan(e.first);

        EXPECT(scan.canonical(nxacc) == e.second);
        EXPECT(scan.reorder(ReducedScan::order_default(), nxacc) == e.second);
    }

    // reordering, and back
    for (const auto& a : expected) {
        for (const auto& b : expected) {
            const auto ab = ReducedScan{a.first}.reorder(b.first, nxacc);
            const auto ba = ReducedScan{b.first}.reorder(a.first, nxacc);

            for (size_t k = 0; k < ab.size(); ++k) {
                EXPECT(ba[ab[k]] == k);
            }
        }
    }
}


CASE("Grid::reorder, from an order") {
    // the renumbering of the order classes, of the grid's sizes; from the grid's own order as reorder(to)
    std::unique_ptr<const Grid> ll(GridFactory::make_from_string("{grid: [10, 10], area: [10, 0, -10, 20]}"));
    std::unique_ptr<const Grid> gg(GridFactory::make_from_string("{grid: O2}"));
    std::unique_ptr<const Grid> hp(GridFactory::make_from_string("{grid: H2}"));

    EXPECT(ll->reorder("i+j+", "i-j-") == order::RegularScan{"i+j+"}.reorder("i-j-", 3, 3));
    const std::vector<size_t> nxacc{0, 20, 44, 68, 88};  // O2 pl: 20, 24, 24, 20
    EXPECT(gg->reorder("i+j+", "i+j-") == order::ReducedScan{"i+j+"}.reorder("i+j-", nxacc));
    EXPECT(hp->reorder(order::HEALPix::NESTED, order::HEALPix::RING) ==
           order::HEALPix(2, order::HEALPix::NESTED).reorder(order::HEALPix::RING));

    for (const auto* grid : {ll.get(), gg.get(), hp.get()}) {
        EXPECT(grid->reorder(grid->order(), grid->order()) == grid->reorder(grid->order()));

        // to an order and back, every point in place
        const auto& from = grid == hp.get() ? order::HEALPix::NESTED : std::string{"i+j+"};
        const auto to    = grid->reorder(grid->order(), from);
        const auto back  = grid->reorder(from, grid->order());
        for (size_t k = 0; k < to.size(); ++k) {
            EXPECT(back[to[k]] == k);
        }
    }

    // unstructured grids have no order to reorder from
    std::unique_ptr<const Grid> un(
        GridFactory::make_from_string("{type: unstructured_ll, longitudes: [0, 1], latitudes: [0, 1]}"));
    EXPECT_THROWS_AS(un->reorder("i+j+", "i+j-"), exception::NotImplemented);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
