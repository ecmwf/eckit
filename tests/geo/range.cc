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
#include "eckit/geo/range/Regular.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


#define EXPECT_APPROX(a, b) EXPECT(::eckit::types::is_approximately_equal<double>((a), (b), 1e-3))


namespace eckit::geo::test {


CASE("range::Regular::make_xy_range") {

    SECTION("make_xy_range") {
        std::unique_ptr<Range> range(range::Regular::make_xy_range(1., -1., 2., -1.));

        EXPECT(range->size() == 4);
        EXPECT(!range->periodic());

        EXPECT_APPROX(range->a(), -1.);
        EXPECT_APPROX(range->b(), 2.);
        EXPECT_APPROX(range->increment(), 1.);

        const auto& values = range->values();

        EXPECT_APPROX(values[0], -1.);
        EXPECT_APPROX(values[1], 0.);
        EXPECT_APPROX(values[2], 1.);
        EXPECT_APPROX(values[3], 2.);
    }
}


CASE("range::Regular::make_longitude_range") {

    SECTION("make_longitude_range") {
        struct test {
            double inc;
            double a;
            double b;
            double ref;
            bool periodic;
            std::vector<double> values;
        };

        for (const auto& test : std::vector<test>{
                 {1., -1., 2., -1., false, {-1, 0, 1, 2}},
                 {90., -90., 90., 0., false, {-90, 0, 90}},
                 {-90., 90., -90., 0., false, {90, 0, -90}},
                 {90., -180., 180., 0., true, {-180, -90, 0, 90}},
                 {45., -180., 180., 0., true, {-180, -135, -90, -45, 0, 45, 90, 135}},
             }) {
            std::unique_ptr<Range> range(range::Regular::make_longitude_range(test.inc, test.a, test.b, test.ref));
            EXPECT(range->periodic() == test.periodic);
            EXPECT(range->size() == test.values.size());

            const auto& values = range->values();
            EXPECT(values.size() == test.values.size());

            for (auto a = test.values.begin(), b = values.begin(); a != test.values.end() && b != values.end();
                 ++a, ++b) {
                EXPECT_APPROX(*a, *b);
            }
        }
    }


    SECTION("make_longitude_range") {
        struct test {
            double inc;
            double a;
            double b;
            double ref;
        };

        for (const auto& test : {
                 test{1., 0., 360., 0.},
                 {1., 0., 360., 0.},
                 {1., 0.5, 359.5, 0.5},
                 {1., 0., 360., 0.5},
             }) {
            std::unique_ptr<Range> range(range::Regular::make_longitude_range(test.inc, test.a, test.b, test.ref));
            EXPECT(range->periodic());
            EXPECT(range->size() == 360);
        }
    }


    SECTION("degenerate") {
        std::unique_ptr<Range> range1(range::Regular::make_longitude_range(1., 1., 1.));

        EXPECT(range1->size() == 1);
        EXPECT(range1->values().front() == 1.);

        std::unique_ptr<Range> range2(range::Regular::make_longitude_range(2., 2., 2.));

        EXPECT(range2->size() == 1);
        EXPECT(range2->values().front() == 2.);

        std::unique_ptr<Range> range3(range::Regular::make_longitude_range(0., 2., 2.));

        EXPECT(range3->size() == 1);
        EXPECT(range3->values().front() == 2.);

        std::unique_ptr<Range> range4(range::Regular::make_longitude_range(0., 2., 2., 1.));

        EXPECT(range4->size() == 1);
        EXPECT(range4->values().front() == 2.);
    }


    SECTION("range [0, 360], cropped") {
        std::unique_ptr<Range> range(range::Regular::make_longitude_range(10., 0., 360.));

        EXPECT(range->size() == 36);
        EXPECT(range->a() == 0.);
        EXPECT(range->b() == 360. - 10.);
        EXPECT(range->periodic());

        const std::unique_ptr<Range> cropped1(range->make_cropped_range(-180., 180.));

        EXPECT(cropped1->size() == 36);
        EXPECT(cropped1->a() == -180.);
        EXPECT(cropped1->b() == 180. - 10.);
        EXPECT(cropped1->periodic());

        const std::unique_ptr<Range> cropped2(range->make_cropped_range(-180., 170.));

        EXPECT(cropped2->size() == 36);
        EXPECT(cropped2->b() == 170.);
        EXPECT(cropped2->periodic());

        const std::unique_ptr<Range> cropped3(range->make_cropped_range(-180., 160.));

        EXPECT(cropped3->size() == 36 - 1);
        EXPECT(cropped3->b() == 160.);
        EXPECT_NOT(cropped3->periodic());
    }


    SECTION("range [0, 180], cropped") {
        std::unique_ptr<Range> range(range::Regular::make_longitude_range(10., 0., 180.));

        EXPECT(range->size() == 19);
        EXPECT_NOT(range->periodic());

        const std::unique_ptr<Range> cropped1(range->make_cropped_range(1., 179.));

        EXPECT(cropped1->size() == 19 - 2);
        EXPECT(cropped1->a() == 10.);
        EXPECT(cropped1->b() == 170.);
        EXPECT_NOT(cropped1->periodic());

        const std::unique_ptr<Range> cropped2(range->make_cropped_range(1., 170.));

        EXPECT(cropped2->size() == 19 - 2);
        EXPECT(cropped2->a() == 10.);
        EXPECT(cropped2->b() == 170.);
        EXPECT_NOT(cropped2->periodic());

        const std::unique_ptr<Range> cropped3(range->make_cropped_range(-180., 180.));

        EXPECT(cropped3->size() == 19);
        EXPECT(cropped3->a() == 0.);
        EXPECT(cropped3->b() == 180.);
        EXPECT_NOT(cropped3->periodic());

        const std::unique_ptr<Range> cropped4(range->make_cropped_range(-190., 170.));

        EXPECT(cropped4->size() == 19 - 1);
        EXPECT(cropped4->a() == 0.);
        EXPECT(cropped4->b() == 170.);
        EXPECT_NOT(cropped4->periodic());
    }
}


CASE("range::Regular::make_latitude_range") {
    SECTION("simple") {
        const std::unique_ptr<Range> range1(range::Regular::make_latitude_range(1., -90., 90., 0.5));

        EXPECT(range1->size() == 180);
        EXPECT(range1->a() == -89.5);
        EXPECT(range1->b() == 89.5);

        const std::unique_ptr<Range> range2(range::Regular::make_latitude_range(-1., 90., -90., 0.5));

        EXPECT(range2->size() == 180);
        EXPECT(range2->a() == 89.5);
        EXPECT(range2->b() == -89.5);
    }
}


CASE("range::Gaussian") {
    std::vector<double> ref{59.44440828916676, 19.87571914744090, -19.87571914744090, -59.44440828916676};


    SECTION("decreasing") {
        auto global = range::GaussianLatitude(2, false);
        EXPECT(global.size() == ref.size());

        size_t i = 0;
        for (const auto& test : global.values()) {
            EXPECT_APPROX(test, ref[i++]);
        }

        std::unique_ptr<Range> cropped1(global.make_cropped_range(50., -50.));
        EXPECT(cropped1->size() == 2);

        EXPECT_APPROX(cropped1->values()[0], ref[1]);
        EXPECT_APPROX(cropped1->values()[1], ref[2]);

        std::unique_ptr<Range> cropped2(global.make_cropped_range(59.445, -59.445));
        std::unique_ptr<Range> cropped3(global.make_cropped_range(59.444, -59.444));
        std::unique_ptr<Range> cropped4(global.make_cropped_range(59.444, -59.445));

        EXPECT(cropped2->size() == 4);
        EXPECT(cropped3->size() == 2);
        EXPECT(cropped4->size() == 3);

        std::unique_ptr<Range> cropped5(global.make_cropped_range(-59.444, -59.445));
        EXPECT(cropped5->size() == 1);

        EXPECT_APPROX(cropped5->values().front(), ref.back());

        std::unique_ptr<Range> cropped6(global.make_cropped_range(90., 0.));
        EXPECT(cropped6->size() == ref.size() / 2);

        EXPECT_APPROX(cropped6->values()[0], ref[0]);
        EXPECT_APPROX(cropped6->values()[1], ref[1]);
    }


    SECTION("increasing") {
        std::vector<double> rev(ref.rbegin(), ref.rend());

        auto global = range::GaussianLatitude(2, true);
        EXPECT(global.size() == rev.size());

        size_t i = 0;
        for (const auto& test : global.values()) {
            EXPECT_APPROX(test, rev[i++]);
        }

        std::unique_ptr<Range> cropped1(global.make_cropped_range(-50., 50.));
        EXPECT(cropped1->size() == 2);

        EXPECT_APPROX(cropped1->values()[0], rev[1]);
        EXPECT_APPROX(cropped1->values()[1], rev[2]);

        std::unique_ptr<Range> cropped2(global.make_cropped_range(-59.445, 59.445));
        std::unique_ptr<Range> cropped3(global.make_cropped_range(-59.444, 59.444));
        std::unique_ptr<Range> cropped4(global.make_cropped_range(-59.445, 59.444));

        EXPECT(cropped2->size() == 4);
        EXPECT(cropped3->size() == 2);
        EXPECT(cropped4->size() == 3);

        std::unique_ptr<Range> cropped5(global.make_cropped_range(-59.445, -59.444));
        EXPECT(cropped5->size() == 1);

        EXPECT_APPROX(cropped5->values().front(), rev.front());

        std::unique_ptr<Range> cropped6(global.make_cropped_range(0., 90.));
        EXPECT(cropped6->size() == rev.size() / 2);

        EXPECT_APPROX(cropped6->values()[0], rev[2]);
        EXPECT_APPROX(cropped6->values()[1], rev[3]);
    }
}


CASE("range::Regular") {
    struct test {
        double inc;
        size_t nlat;
        size_t nlon;
    };

    for (const auto& t : std::vector<test>{
             {30., 7, 12},         //
             {10., 19, 36},        //
             {5., 37, 72},         //
             {2.5, 73, 144},       //
             {2., 91, 180},        //
             {1.8, 101, 200},      //
             {1.6, 113, 225},      //
             {1.5, 121, 240},      //
             {1.4, 129, 258},      //
             {1.25, 145, 288},     //
             {1.2, 151, 300},      //
             {1., 181, 360},       //
             {0.9, 201, 400},      //
             {0.8, 225, 450},      //
             {0.75, 241, 480},     //
             {0.7, 257, 515},      //
             {0.6, 301, 600},      //
             {0.5, 361, 720},      //
             {0.4, 451, 900},      //
             {0.35, 515, 1029},    //
             {0.3, 601, 1200},     //
             {0.25, 721, 1440},    //
             {0.2, 901, 1800},     //
             {0.15, 1201, 2400},   //
             {0.125, 1441, 2880},  //
             {0.1, 1801, 3600},    //
             {0.05, 3601, 7200},   //
         }) {
        std::unique_ptr<Range> lat(range::Regular::make_latitude_range(t.inc, -90., 90., 0.));
        EXPECT_APPROX(lat->increment(), t.inc);
        EXPECT_EQUAL(lat->size(), t.nlat);

        std::unique_ptr<Range> lon(range::Regular::make_longitude_range(t.inc, 0., 360., 0.));
        EXPECT_APPROX(lon->increment(), t.inc);
        EXPECT_EQUAL(lon->size(), t.nlon);
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
