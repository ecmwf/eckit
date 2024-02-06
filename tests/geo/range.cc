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


#include <vector>

#include "eckit/geo/Range.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


#define EXPECT_APPROX(a, b, eps) EXPECT(::eckit::types::is_approximately_equal((a), (b), (eps)))


namespace eckit::test {


using namespace geo;


constexpr auto EPS = 1e-3;


CASE("range::Regular") {
    SECTION("degenerate") {
        EXPECT_THROWS_AS(range::Regular(0, 0., 0.), eckit::AssertionFailed);
        EXPECT_THROWS_AS(range::Regular(0, 0., 10.), eckit::AssertionFailed);

        range::Regular range1(1, 1., 1.);
        EXPECT(range1.size() == 1);
        EXPECT(range1.values().front() == 1.);

        range::Regular range2(2, 2., 2.);
        EXPECT(range2.size() == 1);
        EXPECT(range2.values().front() == 2.);
    }


    SECTION("range [-90, 90]") {
        const range::Regular range(3, -90., 90.);
        EXPECT(range.size() == 3);

        const auto& values = range.values();
        EXPECT(range.size() == values.size());

        EXPECT_APPROX(values[0], -90., EPS);
        EXPECT_APPROX(values[1], 0., EPS);
        EXPECT_APPROX(values[2], 90., EPS);
    }


    SECTION("range [-180, 180]") {
        const range::Regular range1(4, -180., 180.);
        EXPECT(range1.size() == 4);

        const auto& values1 = range1.values();
        EXPECT(range1.size() == values1.size());

        EXPECT_APPROX(values1[0], -180., EPS);
        EXPECT_APPROX(values1[1], -90., EPS);
        EXPECT_APPROX(values1[2], 0., EPS);
        EXPECT_APPROX(values1[3], 90., EPS);

        const range::Regular range2(8, 180., -180.);
        EXPECT(range2.size() == 8);

        const auto& values2 = range2.values();
        EXPECT(range2.size() == values2.size());

        EXPECT_APPROX(values2[0], 180., EPS);
        EXPECT_APPROX(values2[1], 135., EPS);
        EXPECT_APPROX(values2[2], 90., EPS);
        EXPECT_APPROX(values2[3], 45., EPS);
        EXPECT_APPROX(values2[7], -135., EPS);
    }
}


CASE("range::Gaussian") {
    std::vector<double> ref{59.44440828916676, 19.87571914744090, -19.87571914744090, -59.44440828916676};


    SECTION("global") {
        auto global = range::Gaussian(2);
        EXPECT(global.size() == ref.size());

        size_t i = 0;
        for (const auto& test : global.values()) {
            EXPECT_APPROX(test, ref[i++], 1e-12);
        }
    }


    SECTION("crop [50., -50.]") {
        auto cropped = range::Gaussian(2, 50., -50., EPS);
        EXPECT(cropped.size() == ref.size() - 2);

        EXPECT_APPROX(cropped.values()[0], ref[1], EPS);
        EXPECT_APPROX(cropped.values()[1], ref[2], EPS);

        EXPECT(range::Gaussian(2, 59.444, -59.444, 1e-3).size() == 4);
        EXPECT(range::Gaussian(2, 59.444, -59.444, 1e-6).size() == 2);
        EXPECT(range::Gaussian(2, 59.444, -59.445, 1e-6).size() == 3);

        auto single = range::Gaussian(2, -59.444, -59.444, EPS);
        EXPECT(single.size() == 1);

        EXPECT_APPROX(single.values().front(), ref.back(), EPS);
    }


    SECTION("crop [90., 0.]") {
        constexpr auto eps = 1e-3;

        auto cropped = range::Gaussian(2, 90., 0., eps);
        EXPECT(cropped.size() == ref.size() / 2);

        EXPECT_APPROX(cropped.values()[0], ref[0], eps);
        EXPECT_APPROX(cropped.values()[1], ref[1], eps);
    }
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
