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

#include "eckit/geo/figure/Sun.h"
#include "eckit/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


struct F : std::unique_ptr<Figure> {
    explicit F(Figure* ptr) : unique_ptr(ptr) { ASSERT(operator bool()); }
};


CASE("Sun") {
    F f1(FigureFactory::build(spec::Custom{{"figure", "sun"}}));
    F f2(FigureFactory::build(spec::Custom{{"r", figure::DatumSun::radius}}));
    F f3(new figure::Sun);

    EXPECT(*f1 == *f2);
    EXPECT(*f1 == *f3);

    EXPECT(f1->spherical());
    EXPECT(types::is_approximately_equal(f1->R(), 695990000.));
    EXPECT(types::is_approximately_equal(f1->eccentricity(), 0.));

    EXPECT(f1->spec_str() == R"({"figure":"sun"})");
}


CASE("Sun surface area") {
    EXPECT(types::is_approximately_equal(figure::SUN.area() * 1e-6, 6087176064903., 1.));
    EXPECT(types::is_strictly_greater(figure::DatumSun::radius, 100. * 6371229.));
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
