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


#include "eckit/geo/util/Qhull.h"
#include "eckit/testing/Test.h"


//----------------------------------------------------------------------------------------------------------------------


namespace eckit::test {


CASE("QHull error codes") {
    using geo::util::Qhull;


    SECTION("QH6214 qhull input error: not enough points(1) to construct initial simplex (need 3)") {
        try {
            Qhull(2, {1, 1}, "Qt");
            EXPECT(false);
        }
        catch (const Qhull::QhullException& e) {
            EXPECT(e.errorCode == 6214);
        }
    }
}


}  // namespace eckit::test


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
