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

#include "eckit/geo/figure/Earth.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::test {


struct F : std::shared_ptr<Figure> {
    explicit F(Figure* ptr) : shared_ptr(ptr) { ASSERT(operator bool()); }
};


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


CASE("Area") {
    struct test_t {
        test_t(const std::string& _figure, double _area) :
            figure(FigureFactory::build(spec::Custom{{"figure", _figure}})), area(_area) {}

        const F figure;
        const double area;
    };

    for (const auto& test : std::vector<test_t>{
             {"earth", 510101140.},  //[km^2]
             {"wgs84", 510065621.},
             {"grs80", 510065621.},
         }) {
        EXPECT(types::is_approximately_equal(test.figure->area() * 1e-6 /*[km^2]*/, test.area, 1.));
    }
}


}  // namespace eckit::geo::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
