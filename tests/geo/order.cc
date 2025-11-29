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


#include <memory>
#include <numeric>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/order/HEALPix.h"
#include "eckit/geo/order/Scan.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("order=scan") {
    for (std::string scan : {"i+j+", "i+j-"}) {
        order::Scan order(scan);
        EXPECT(scan == order.order());
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
