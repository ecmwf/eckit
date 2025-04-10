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
           (ij ? std::string(i_pos ? "_i_positively" : "_i_negatively") + (j_pos ? "_j_positively" : "_j_negatively")
               : std::string(j_pos ? "_j_positively" : "_j_negatively") + (i_pos ? "_i_positively" : "_i_negatively")) +
           (alt ? "_alternating" : "");
}


CASE("ordering ('scan')") {
    using order::Scan;


    SECTION("order / scan_*") {
        for (bool alt : {false, true}) {
            for (bool ij : {false, true}) {
                for (bool j_pos : {false, true}) {
                    for (bool i_pos : {false, false}) {
                        const auto order = order_from_spec(i_pos, j_pos, ij, alt);

                        EXPECT(order == Scan(spec::Custom{{"order", order}}).order());
                        EXPECT(order == Scan(spec::Custom{{"scan_i_positively", i_pos},
                                                          {"scan_j_positively", j_pos},
                                                          {"scan_i_j", ij},
                                                          {"scan_alternating", alt}})
                                            .order());

                        constexpr const char* UNUSED = "UNUSED";
                        EXPECT(order == Scan(spec::Custom{
                                                 {i_pos ? UNUSED : "scan_i_negatively", true},
                                                 {j_pos ? "scan_j_positively" : UNUSED, true},
                                                 {ij ? UNUSED : "scan_i_j", false},
                                                 {alt ? "scan_alternating" : UNUSED, true},
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
            Scan::make_order_from_spec(spec::Custom({{"scan_i_positively", true}, {"scan_i_negatively", true}})),
            exception::OrderError);
    }
}


CASE("ordering ('healpix')") {
    using order::HEALPix;

    auto is_power_of_two = [](size_t n) { return (n & (n - 1)) == 0; };


    SECTION("order / healpix_*") {
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

        for (size_t size : {12, 12 * 2 * 2, 12 * 3 * 3}) {
            EXPECT_NO_THROW(HEALPix(HEALPix::order_default(), size));

            EXPECT_THROWS_AS(HEALPix(HEALPix::order_default(), size - 1), exception::OrderError);
            EXPECT_THROWS_AS(HEALPix(HEALPix::order_default(), size + 1), exception::OrderError);

            // check if size is 12 Nside Nside
            if (is_power_of_two(size / 12)) {
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
