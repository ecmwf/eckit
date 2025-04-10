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


#include <numeric>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Order.h"
#include "eckit/geo/order/HEALPix.h"
#include "eckit/geo/order/Scan.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


Order::value_type order_from_spec(bool i_pos, bool j_pos, bool ij, bool alt) {
    return "scan" +
           (ij ? std::string(i_pos ? "-i-positively" : "-i-negatively") + (j_pos ? "-j-positively" : "-j-negatively")
               : std::string(j_pos ? "-j-positively" : "-j-negatively") + (i_pos ? "-i-positively" : "-i-negatively")) +
           (alt ? "-alternating" : "");
}


CASE("ordering scan") {
    using order::Scan;


    SECTION("order=scan-*") {
        for (bool alt : {false, true}) {
            for (bool ij : {false, true}) {
                for (bool j_pos : {false, true}) {
                    for (bool i_pos : {false, false}) {
                        const auto order = order_from_spec(i_pos, j_pos, ij, alt);

                        EXPECT(order == Scan(spec::Custom{{"order", order}}).order());
                        EXPECT(order == Scan(spec::Custom{{"scan-i-positively", i_pos},
                                                          {"scan-j-positively", j_pos},
                                                          {"scan-i-j", ij},
                                                          {"scan-alternating", alt}})
                                            .order());

                        constexpr const char* UNUSED = "UNUSED";
                        EXPECT(order == Scan(spec::Custom{
                                                 {i_pos ? UNUSED : "scan-i-negatively", true},
                                                 {j_pos ? "scan-j-positively" : UNUSED, true},
                                                 {ij ? UNUSED : "scan-i-j", false},
                                                 {alt ? "scan-alternating" : UNUSED, true},
                                             })
                                            .order());

                        Scan scan(order, 2, 3);
                        auto no_reorder = scan.reorder(order);

                        std::vector<size_t> expected(scan.size());
                        std::iota(expected.begin(), expected.end(), 0);
                        EXPECT(expected == no_reorder);
                        EXPECT(expected == scan.no_reorder());
                    }
                }
            }
        }
    }


    SECTION("exceptions") {
        EXPECT_THROWS_AS(
            Scan::make_order_from_spec(spec::Custom({{"scan-i-positively", true}, {"scan-i-negatively", true}})),
            exception::OrderError);
    }
}


CASE("ordering healpix") {
    using order::HEALPix;

    auto is_power_of_two = [](size_t n) { return (n & (n - 1)) == 0; };


    SECTION("order=ring/nested") {
        EXPECT(HEALPix::order_default() == HEALPix(spec::Custom{}).order());
        EXPECT(HEALPix::ring == HEALPix(spec::Custom{{"order", HEALPix::ring}}).order());
        EXPECT(HEALPix::nested == HEALPix(spec::Custom{{"order", HEALPix::nested}}).order());

        for (const auto& type : {HEALPix::order_default(), HEALPix::ring, HEALPix::nested}) {
            HEALPix order(type);
            auto no_reorder = order.reorder(type);

            std::vector<size_t> expected(order.size());
            std::iota(expected.begin(), expected.end(), 0);
            EXPECT(expected == no_reorder);
            EXPECT(expected == order.no_reorder());
        }
    }


    SECTION("exceptions") {
        EXPECT_THROWS_AS(HEALPix(spec::Custom{{"order", "?"}}), exception::OrderError);

        EXPECT_THROWS_AS(HEALPix(HEALPix::order_default(), 0), exception::OrderError);

        for (size_t Nside : {1, 2, 3}) {
            const auto size = 12 * Nside * Nside;

            EXPECT_NO_THROW(HEALPix(HEALPix::order_default(), size));

            EXPECT_THROWS_AS(HEALPix(HEALPix::order_default(), size - 1), exception::OrderError);
            EXPECT_THROWS_AS(HEALPix(HEALPix::order_default(), size + 1), exception::OrderError);

            if (is_power_of_two(Nside)) {
                EXPECT_NO_THROW(HEALPix(HEALPix::nested, size));
            }
            else {
                EXPECT_THROWS_AS(HEALPix(HEALPix::nested, size), exception::OrderError);
            }
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
