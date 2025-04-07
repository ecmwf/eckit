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
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("ordering ('scan')") {
    for (bool alt : {false, true}) {
        for (bool ij : {true, false}) {
            for (bool j_pos : {false, true}) {
                for (bool i_pos : {true, false}) {
                    spec::Custom spec({{"scan_i_positively", i_pos},
                                       {"scan_j_positively", j_pos},
                                       {"scan_ij", ij},
                                       {"scan_alternating", alt}});

                    Order::value_type ordering("scan");
                    ordering += i_pos ? "_i_positively" : "_i_negatively";
                    ordering += j_pos ? "_j_positively" : "_j_negatively";
                    ordering += ij ? "_ij" : "_ji";
                    ordering += alt ? "_i_alternating_direction" : "_i_single_direction";

                    EXPECT(Order::make_ordering_from_spec(spec) == ordering);
                }
            }
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
