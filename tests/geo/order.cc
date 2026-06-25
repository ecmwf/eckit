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


#include <map>
#include <numeric>
#include <string>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/order/HEALPix.h"
#include "eckit/geo/order/Scan.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("order=scan") {
    using order::Scan;

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
        const Scan scan(order);
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
}


CASE("healpix") {
    using order::HEALPix;

    auto is_power_of_two = [](size_t n) { return (n & (n - 1)) == 0; };


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

        for (size_t Nside : {1, 2, 3}) {
            const auto size = 12 * Nside * Nside;

            EXPECT_NO_THROW(HEALPix{});
            EXPECT_NO_THROW(HEALPix{HEALPix::order_default()});
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
