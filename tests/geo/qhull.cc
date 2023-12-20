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
#include "eckit/exception/Exceptions.h"
#include "eckit/testing/Test.h"


//----------------------------------------------------------------------------------------------------------------------


namespace eckit::test {


CASE("QHull exceptions") {
    using geo::util::Qhull;


    SECTION("Assertion failed: 0 < N && coord.size() % N == 0 in Qhull") {
        for (size_t N : {2, 3, 4}) {
            Qhull::coord_t coord((N + 1) * N + 1, 1.);
            ASSERT(coord.size() % N != 0);
            EXPECT_THROWS_AS(Qhull(N, coord, "Qt"), AssertionFailed);
        }
    }


    SECTION("QH6050 qhull error: dimension 1 must be > 1") {
        try {
            Qhull(1, {1, 1}, "Qt");
            EXPECT(false);
        }
        catch (const Qhull::Exception& e) {
            EXPECT(e.errorCode == 6050);
        }
    }


    SECTION("QH6154 Qhull precision error: Initial simplex is flat (facet 1 is coplanar with the interior point)") {
        try {
            Qhull(2, {1, 1, 2, 2, 3, 3, 4, 4}, "Qt");
            EXPECT(false);
        }
        catch (const Qhull::Exception& e) {
            EXPECT(e.errorCode == 6154);
        }
    }


    SECTION("QH6214 qhull input error: not enough points(1) to construct initial simplex (need 3)") {
        try {
            Qhull(2, {1, 1}, "Qt");
            EXPECT(false);
        }
        catch (const Qhull::Exception& e) {
            EXPECT(e.errorCode == 6214);
        }
    }


    SECTION("QH6412 qhull input error (qh_initqhull_globals): expecting between 1 and 2147483631 points") {
        try {
            Qhull(2, {}, "Qt");
            EXPECT(false);
        }
        catch (const Qhull::Exception& e) {
            EXPECT(e.errorCode == 6412);
        }
    }


    SECTION("QH6421 qhull internal error (qh_maxsimplex): qh.MAXwidth required for qh_maxsimplex") {
        try {
            Qhull(2, {1, 1, 1, 1, 1, 1}, "Qt");
            EXPECT(false);
        }
        catch (const Qhull::Exception& e) {
            EXPECT(e.errorCode == 6421);
        }
    }
}


}  // namespace eckit::test


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
