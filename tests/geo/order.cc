// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include <cstddef>
#include <map>
#include <numeric>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"
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
    for (const auto& [order, points] : std::map<std::string, RegularScan::renumber_type>{
             {"i+j-", {0, 1, 2, 3, 4, 5}},
             {"i+j+", {3, 4, 5, 0, 1, 2}},
             {"i-j-", {2, 1, 0, 5, 4, 3}},
             {"i-j+", {5, 4, 3, 2, 1, 0}},
             {"j-i+", {0, 3, 1, 4, 2, 5}},
             {"j+i+", {3, 0, 4, 1, 5, 2}},
             {"j-i-", {2, 5, 1, 4, 0, 3}},
             {"j+i-", {5, 2, 4, 1, 3, 0}},
             {"i+-j-", {0, 1, 2, 5, 4, 3}},
             {"j-+i+", {0, 3, 4, 1, 2, 5}},
         }) {
        EXPECT(RegularScan{order}.reorder(RegularScan::order_default(), ni, nj) == points);
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

        EXPECT(HEALPix::order_default() == HEALPix(spec::Custom{}).order());
        EXPECT(HEALPix::RING == HEALPix(spec::Custom{{"order", HEALPix::RING}}).order());
        EXPECT(HEALPix::NESTED == HEALPix(spec::Custom{{"order", HEALPix::NESTED}}).order());

        for (const auto& type : {HEALPix::order_default(), HEALPix::RING, HEALPix::NESTED}) {
            HEALPix order(type);
            auto no_reorder = order.reorder(type, Nside);

            std::vector<size_t> expected(no_reorder.size());
            std::iota(expected.begin(), expected.end(), 0);
            EXPECT(expected == no_reorder);
        }
    }


    SECTION("exceptions") {
        EXPECT_THROWS_AS(HEALPix(spec::Custom{{"order", "?"}}), exception::OrderError);

        EXPECT_THROWS_AS(HEALPix("?"), exception::OrderError);

        // nested: Nside a power of 2, up to 2^13 (checked before any int arithmetic)
        for (size_t Nside : {size_t{0}, size_t{3}, size_t{1} << 14, size_t{1} << 40}) {
            EXPECT_THROWS_AS(HEALPix::Renumber{Nside}, exception::OrderError);
        }
        EXPECT_NO_THROW(HEALPix::Renumber{size_t{1} << 13});

        for (size_t Nside : {1, 2, 3}) {
            const auto size = 12 * Nside * Nside;

            EXPECT_NO_THROW(HEALPix{});
            EXPECT_NO_THROW(HEALPix{HEALPix::order_default()});
        }
    }
}


CASE("order=reduced scan") {
    using order::ReducedScan;

    EXPECT(ReducedScan{}.order() == ReducedScan::order_default());
    EXPECT(ReducedScan{""}.order() == "i+j-");

    // rows of points: consecutive along rows, rows not alternating direction
    for (const std::string order : {"i+j-", "i+j+", "i-j-", "i-j+"}) {
        EXPECT_NO_THROW(ReducedScan{order});
    }

    for (const std::string order : {"j+i+", "j-i-", "i+-j-", "i-+j+", "j+-i+", "ring", "x"}) {
        EXPECT_THROWS_AS(ReducedScan{order}, exception::OrderError);
    }

    // rows of 2, 0, 3 and 1 points (rows in the default order, north to south): index in the default order of each
    // point
    const pl_type pl{2, 0, 3, 1};

    const std::map<std::string, ReducedScan::renumber_type> expected{
        {"i+j-", {0, 1, 2, 3, 4, 5}},
        {"i+j+", {5, 2, 3, 4, 0, 1}},
        {"i-j-", {1, 0, 4, 3, 2, 5}},
        {"i-j+", {5, 4, 3, 2, 1, 0}},
    };

    for (const auto& [order, canonical] : expected) {
        const ReducedScan scan(order);

        EXPECT(scan.canonical(pl) == canonical);
        EXPECT(scan.reorder(ReducedScan::order_default(), pl) == canonical);
    }

    // reordering, and back
    for (const auto& [a, _a] : expected) {
        for (const auto& [b, _b] : expected) {
            const auto ab = ReducedScan{a}.reorder(b, pl);
            const auto ba = ReducedScan{b}.reorder(a, pl);

            for (size_t k = 0; k < ab.size(); ++k) {
                EXPECT(ba[ab[k]] == k);
            }
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
