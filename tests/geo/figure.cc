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

#include "eckit/geo/Figure.h"
#include "eckit/geo/figure/Earth.h"
#include "eckit/geo/figure/OblateSpheroid.h"
#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


struct F : std::unique_ptr<Figure> {
    explicit F(Figure* ptr) : unique_ptr(ptr) { ASSERT(unique_ptr::operator bool()); }
};


CASE("Sphere") {
    F f1(FigureFactory::build(spec::Custom{{"R", 1.}}));
    F f2(FigureFactory::build(spec::Custom{{"a", 1.}, {"b", 1.}}));
    F f3(new figure::Sphere(1.));

    EXPECT_THROWS_AS(figure::Sphere(-1.), BadValue);

    EXPECT(*f1 == *f2);
    EXPECT(*f1 == *f3);

    auto e = f1->eccentricity();
    EXPECT(types::is_approximately_equal(e, 0.));

    EXPECT(f1->spec_str() == R"({"r":1})");
}


CASE("Oblate spheroid") {
    F f1(FigureFactory::build(spec::Custom{{"b", 0.5}, {"a", 1.}}));
    F f2(new figure::OblateSpheroid(1., 0.5));

    EXPECT_THROWS_AS(figure::OblateSpheroid(0.5, 1.), BadValue);  // prolate spheroid
    EXPECT_THROWS_AS(figure::OblateSpheroid(1., -1.), BadValue);

    EXPECT(*f1 == *f2);

    auto e = f1->eccentricity();
    EXPECT(types::is_strictly_greater(e, 0.));

    EXPECT(f1->spec_str() == R"({"a":1,"b":0.5})");
}


CASE("Earth") {
    F f1(FigureFactory::build(spec::Custom{{"r", 6371229.}}));
    F f2(new figure::Earth);

    EXPECT(*f1 == *f2);
    EXPECT(f1->spec_str() == R"({"r":6371229})");
    EXPECT(types::is_approximately_equal(f1->R(), 6371229., 1e-8));

    F f4(FigureFactory::build(spec::Custom{{"figure", "wgs84"}}));

    EXPECT(f4->spec_str() == R"({"figure":"wgs84"})");
    EXPECT(types::is_approximately_equal(1. / f4->flattening(), 298.257223563, 1e-8));
    EXPECT_THROWS_AS(f4->R(), BadValue);
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
