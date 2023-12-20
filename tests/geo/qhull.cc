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


CASE("Qhull errors/exceptions") {
    using geo::util::Qhull;


    SECTION("input errors") {
        for (size_t N : {2, 3, 4}) {
            Qhull::coord_t coord((N + 1) * N + 1, 1.);
            ASSERT(coord.size() % N != 0);
            EXPECT_THROWS_AS(Qhull(N, coord, "Qt"), AssertionFailed);  // 0 < N && coord.size() % N == 0
        }
    }


    SECTION("qhull errors") {
        struct {
            const int errorCode;
            const std::string what;
            const std::string command;
            const size_t N;
            const Qhull::coord_t coord;
        } static const tests[] = {
            {6050, "QH6050 qhull error: dimension 1 must be > 1", "Qt", 1, {1, 1}},
            {6154, "QH6154 Qhull precision error: Initial simplex is flat", "Qt", 2, {1, 1, 2, 2, 3, 3}},
            {6214, "QH6214 qhull input error: not enough points", "Qt", 2, {1, 1}},
            {6412, "QH6412 qhull input error (qh_initqhull_globals)", "Qt", 2, {}},
            {6421, "QH6421 qhull internal error (qh_maxsimplex)", "Qt", 2, {1, 1, 1, 1, 1, 1}},
        };

        for (const auto& test : tests) {
            try {
                Qhull(test.N, test.coord, test.command);
                EXPECT(false);
            }
            catch (const Qhull::Exception& e) {
                EXPECT_EQUAL(test.errorCode, e.errorCode);
                EXPECT_EQUAL(test.what, std::string(e.what(), test.what.length()));
            }
        }
    }
}


}  // namespace eckit::test


//----------------------------------------------------------------------------------------------------------------------


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
