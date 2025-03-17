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


#include "eckit/testing/Test.h"

#include "mir/util/Polygon2.h"


namespace mir::tests::unit {


inline bool is_approximately_equal(double a, double b) {
    return eckit::types::is_approximately_equal(a, b, 1e-6);
}


CASE("Polygon2") {
    using util::Polygon2;

    const Polygon2 clipper{{-1, -1}, {1, -1}, {1, 1}, {-1, 1}};


    SECTION("Polygon2 is empty") {
        Polygon2 poly;
        poly.clip(clipper);
        EXPECT(poly.empty());
    }


    SECTION("Polygon2 is completely covers the clipping polygon") {
        Polygon2 poly{{-2, 0}, {0, -2}, {2, 0}, {0, 2}};
        poly.clip(clipper);
        EXPECT(poly == clipper);
    }


    SECTION("Polygon exactly aligns with clipping boundary") {
        auto poly = clipper;
        EXPECT(poly == clipper);

        poly.push_back((clipper.back() + clipper.front()) * 0.5);
        EXPECT(poly != clipper);

        poly.clip(clipper);

        EXPECT(poly == clipper);
    }


    SECTION("Polygon2 is completely inside the clipping polygon") {
        auto poly     = Polygon2{{0, 0}, {0.5, 0.5}, {-0.5, 0.5}};
        auto expected = poly;
        poly.clip(clipper);

        EXPECT(poly == expected);
    }


    SECTION("Vertix/vertices outside the clipping polygon (1)") {
        auto poly = Polygon2{{-1., 0.5}, {-2., 0.}, {-1., -2.}, {0.5, -1.}};
        poly.clip(clipper);

        Polygon2 expected{{-1., -1.}, {0.5, -1.}, {-1., 0.5}};
        EXPECT(poly == expected);
        EXPECT(is_approximately_equal(poly.area(), 1.125));
    }


    SECTION("Vertix/vertices outside the clipping polygon (2)") {
        Polygon2 poly{{0., 0.}, {2., 0.}, {1., 2.}};
        poly.clip(clipper);

        Polygon2 expected{{0.5, 1.}, {0., 0.}, {1., 0.}, {1., 1.}};
        EXPECT(poly == expected);
    }


    SECTION("Vertix/vertices outside the clipping polygon (3)") {
        auto poly = Polygon2{{1., 1.5}, {2., 0.5}, {-0.5, -2.}, {-2., 0.}};
        poly.clip(clipper);

        Polygon2 expected{{-1., 0.5}, {0., 1.}, {1., 1.}, {1., -0.5}, {0.5, -1.}, {-1., -1.}};
        EXPECT(poly == expected);
        EXPECT(is_approximately_equal(poly.area(), 3.625));
    }


    SECTION("Vertix/vertices outside the clipping polygon (4)") {
        auto poly = Polygon2{{-2, 0}, {0, -2}, {2, 0}, {0, 2}};
        poly.clip(clipper);

        EXPECT(poly == clipper);
        EXPECT(is_approximately_equal(poly.area(), 4.));
    }


    SECTION("Vertix/vertices outside the clipping polygon (5)") {
        Polygon2 poly{{1., -0.5}, {1., 0.5}, {-1., 0.}};
        Polygon2 clipper{{0.5, 0.}, {0., 0.5}, {0., 0.}};
        poly.clip(clipper);

        Polygon2 expected{{0., 0.}, {0.5, 0.}, {0.2, 0.3}, {0., 0.25}};
        EXPECT(poly == expected);
    }


    SECTION("Polygon2 is completely outside the clipping polygon") {
        auto poly = Polygon2{{2, 2}, {3, 3}, {3, 2}};
        poly.clip(clipper);

        EXPECT(poly.empty());
        EXPECT(is_approximately_equal(poly.area(), 0.));
    }


    SECTION("Concave polygon where part is clipped but part remains inside.") {
        auto poly = Polygon2{{-2., 2.}, {-2., -2.}, {2., -2.}};
        poly.clip(clipper);

        Polygon2 expected{{-1, -1}, {1, -1}, {-1, 1}};
        EXPECT(poly == expected);
        EXPECT(is_approximately_equal(poly.area(), 2.));
    }


    SECTION("Polygon intersection") {
        Polygon2 poly{{0.000304552, -5.32E-06}, {0, 0.026185917}, {0., 0.}};
        Polygon2 clipper{{0.000304598, 0.008724209}, {-0.000304598, 0.008724209}, {0, -0.008726866}};
        poly.clip(clipper);

        Polygon2 expected{{0.0001522757595430324, -2.659995799630596e-06},
                          {0.0002436488701595806, 0.005232302171885906},
                          {0.0002030449380766552, 0.008724209},
                          {0, 0.008724209},
                          {0, 0}};
        EXPECT(poly == expected);
    }


    SECTION("Polygon simplify") {
        Polygon2 poly{{0., -1.}, {1., 1.}, {-1., 1.}, {0., -1.}};
        poly.simplify();

        Polygon2 expected{{-1., 1.}, {0., -1.}, {1., 1.}};
        EXPECT(poly == expected);
    }
}


}  // namespace mir::tests::unit


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
