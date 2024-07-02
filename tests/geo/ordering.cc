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


#include "eckit/geo/Ordering.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"


namespace eckit::geo::test {


CASE("ordering ('scan')") {
    int ordering = 0;

    for (bool scan_alternating : {false, true}) {
        for (bool scan_ij : {true, false}) {
            for (bool j_plus : {false, true}) {
                for (bool i_plus : {true, false}) {
                    spec::Custom spec({{"scan_i_plus", i_plus},
                                       {"scan_j_plus", j_plus},
                                       {"scan_ij", scan_ij},
                                       {"scan_alternating", scan_alternating}});

                    EXPECT(static_cast<int>(make_ordering_from_spec(spec)) == ordering);

                    ++ordering;
                }
            }
        }
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
