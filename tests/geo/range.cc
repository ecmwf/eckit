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
#include <vector>

#include "eckit/geo/range/GaussianLatitude.h"
#include "eckit/geo/range/RegularLongitude.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"
#include "eckit/types/Fraction.h"


#define EXPECT_APPROX(a, b, eps) EXPECT(::eckit::types::is_approximately_equal<double>((a), (b), (eps)))


namespace eckit::geo::test {


constexpr auto EPS = 1e-3;


std::ostream& operator<<(std::ostream& out, const std::vector<double>& v) {
    const char* sep = "";
    for (const auto& e : v) {
        out << sep << e;
        sep = ", ";
    }
    return out;
}


CASE("range::RegularLongitude") {
    SECTION("simple") {
        const range::RegularLongitude range1(1., -1., 2.);

        EXPECT(range1.size() == 4);

        EXPECT_APPROX(range1.a(), -1., EPS);
        EXPECT_APPROX(range1.b(), 2., EPS);
        EXPECT_APPROX(range1.increment(), 1., EPS);

        const auto& values = range1.values();

        EXPECT_APPROX(values[0], -1., EPS);
        EXPECT_APPROX(values[1], 0., EPS);
        EXPECT_APPROX(values[2], 1., EPS);
        EXPECT_APPROX(values[3], 2., EPS);
    }


    SECTION("degenerate") {
        EXPECT_THROWS_AS(range::RegularLongitude(static_cast<size_t>(0), 0., 0.), eckit::AssertionFailed);
        EXPECT_THROWS_AS(range::RegularLongitude(static_cast<size_t>(0), 0., 10.), eckit::AssertionFailed);

        range::RegularLongitude range1(static_cast<size_t>(1), 1., 1.);

        EXPECT(range1.size() == 1);
        EXPECT(range1.values().front() == 1.);

        range::RegularLongitude range2(static_cast<size_t>(2), 2., 2.);

        EXPECT(range2.size() == 1);
        EXPECT(range2.values().front() == 2.);
    }


    SECTION("range [-90, 90]") {
        const range::RegularLongitude range(static_cast<size_t>(3), -90., 90.);

        EXPECT(range.size() == 3);

        const auto& values = range.values();

        EXPECT(range.size() == values.size());

        EXPECT_APPROX(values[0], -90., EPS);
        EXPECT_APPROX(values[1], 0., EPS);
        EXPECT_APPROX(values[2], 90., EPS);
    }


    SECTION("range [-180, 180]") {
        const range::RegularLongitude range1(static_cast<size_t>(4), -180., 180.);

        EXPECT(range1.size() == 4);

        const auto& values1 = range1.values();

        EXPECT(range1.size() == values1.size());

        EXPECT_APPROX(values1[0], -180., EPS);
        EXPECT_APPROX(values1[1], -90., EPS);
        EXPECT_APPROX(values1[2], 0., EPS);
        EXPECT_APPROX(values1[3], 90., EPS);

        const range::RegularLongitude range2(static_cast<size_t>(8), 180., -180.);

        EXPECT(range2.size() == 8);

        const auto& values2 = range2.values();

        EXPECT(range2.size() == values2.size());

        EXPECT_APPROX(values2[0], 180., EPS);
        EXPECT_APPROX(values2[1], 135., EPS);
        EXPECT_APPROX(values2[2], 90., EPS);
        EXPECT_APPROX(values2[3], 45., EPS);
        EXPECT_APPROX(values2[7], -135., EPS);
    }


    SECTION("range [0, 360], cropped") {
        auto range = range::RegularLongitude(static_cast<size_t>(36), 0., 360.);
        // const std::unique_ptr<Range> range1(range.crop(-180., 180.));

        // std::cout << range1->values() << std::endl;
        // EXPECT(range1->size() == 36);
        // EXPECT(range1->a() == -180.);
        // EXPECT(range1->b() == 180.);

        // const std::unique_ptr<Range> range2(range.crop(-180., 170.));

        // EXPECT(range2->size() == 36);
        // EXPECT(range2->b() == 180.);

        // const std::unique_ptr<Range> range3(range.crop(-180., 160.));

        // EXPECT(range3->size() == 36 - 1);
        // EXPECT(range3->b() == 160.);
    }


    SECTION("range [0, 180], cropped") {
        auto range = range::RegularLongitude(static_cast<size_t>(19), 0., 180.);
        const std::unique_ptr<Range> range1(range.crop(1., 179.));

        EXPECT(range1->size() == 19 - 2);
        EXPECT(range1->a() == 10.);
        EXPECT(range1->b() == 170.);

        const std::unique_ptr<Range> range2(range.crop(1., 170.));

        EXPECT(range2->size() == 19 - 2);
        EXPECT(range2->a() == 10.);
        EXPECT(range2->b() == 170.);

        // const std::unique_ptr<Range> range3(range.crop(-180., 180.));

        // std::cout << range3->values() << std::endl;
        // EXPECT(range3->size() == 19);
        // EXPECT(range3->a() == 0.);
        // EXPECT(range3->b() == 180.);

        // const std::unique_ptr<Range> range4(range.crop(-190., 170.));

        // std::cout << range4->values() << std::endl;
        // EXPECT(range4->size() == 2);
        // EXPECT(range4->a() == -190.);
        // EXPECT(range4->b() == -180.);
    }
}


CASE("range::Gaussian") {
    std::vector<double> ref{59.44440828916676, 19.87571914744090, -19.87571914744090, -59.44440828916676};


    SECTION("global") {
        auto global = range::GaussianLatitude(2, false);
        EXPECT(global.size() == ref.size());

        size_t i = 0;
        for (const auto& test : global.values()) {
            EXPECT_APPROX(test, ref[i++], 1e-12);
        }
    }


    SECTION("crop [50., -50.]") {
        std::unique_ptr<Range> cropped(range::GaussianLatitude(2, false).crop(50., -50.));
        EXPECT(cropped->size() == ref.size() - 2);

        EXPECT_APPROX(cropped->values()[0], ref[1], EPS);
        EXPECT_APPROX(cropped->values()[1], ref[2], EPS);

        EXPECT(std::unique_ptr<Range>(range::GaussianLatitude(2, false, 1e-3).crop(59.444, -59.444))->size() == 4);
        EXPECT(std::unique_ptr<Range>(range::GaussianLatitude(2, false, 1e-6).crop(59.444, -59.444))->size() == 2);
        EXPECT(std::unique_ptr<Range>(range::GaussianLatitude(2, false, 1e-6).crop(59.444, -59.445))->size() == 3);

        std::unique_ptr<Range> single(range::GaussianLatitude(2, false, EPS).crop(-59.444, -59.444));
        EXPECT(single->size() == 1);

        EXPECT_APPROX(single->values().front(), ref.back(), EPS);
    }


    SECTION("crop [90., 0.]") {
        std::unique_ptr<Range> cropped(range::GaussianLatitude(2, false, EPS).crop(90., 0.));
        EXPECT(cropped->size() == ref.size() / 2);

        EXPECT_APPROX(cropped->values()[0], ref[0], EPS);
        EXPECT_APPROX(cropped->values()[1], ref[1], EPS);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
