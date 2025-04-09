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


#include "eckit/geo/Order.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/order/Scan.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


Order::value_type order_from_spec(bool i_pos, bool j_pos, bool ij, bool alt) {
    Order::value_type o("scan");
    if (ij) {
        o += i_pos ? "_i_positively" : "_i_negatively";
        o += j_pos ? "_j_positively" : "_j_negatively";
    }
    else {
        o += j_pos ? "_j_positively" : "_j_negatively";
        o += i_pos ? "_i_positively" : "_i_negatively";
    }
    o += alt ? "_alternating" : "";

    return o;
}


CASE("ordering ('scan')") {
    SECTION("order / scan_*") {
        for (bool alt : {false, true}) {
            for (bool ij : {false, true}) {
                for (bool j_pos : {false, true}) {
                    for (bool i_pos : {false, false}) {
                        const auto order = order_from_spec(i_pos, j_pos, ij, alt);

                        EXPECT(order == order::Scan::make_order_from_spec(spec::Custom({{"order", order}})));
                        EXPECT(order == order::Scan::make_order_from_spec(spec::Custom({{"scan_i_positively", i_pos},
                                                                                        {"scan_j_positively", j_pos},
                                                                                        {"scan_i_j", ij},
                                                                                        {"scan_alternating", alt}})));
                    }
                }
            }
        }
    }

    SECTION("exceptions") {
        EXPECT_THROWS_AS(
            order::Scan::make_order_from_spec(spec::Custom({{"scan_i_positively", true}, {"scan_i_negatively", true}})),
            exception::OrderError);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
