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


#include <iostream>
#include <vector>

#include "eckit/geo/range/Gaussian.h"
#include "eckit/geo/range/Regular.h"
#include "eckit/testing/Test.h"
#include "eckit/types/FloatCompare.h"


#define EXPECT_APPROX(a, b, eps) EXPECT(::eckit::types::is_approximately_equal((a), (b), (eps)))


namespace eckit::test {


using namespace geo;


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
        constexpr auto eps = 1e-3;

        auto cropped = range::Gaussian(2, 50., -50., eps);
        EXPECT(cropped.size() == ref.size() - 2);

        EXPECT_APPROX(cropped.values()[0], ref[1], eps);
        EXPECT_APPROX(cropped.values()[1], ref[2], eps);

        EXPECT(range::Gaussian(2, 59.444, -59.444, 1e-3).size() == 4);
        EXPECT(range::Gaussian(2, 59.444, -59.444, 1e-6).size() == 2);
        EXPECT(range::Gaussian(2, 59.444, -59.445, 1e-6).size() == 3);

        auto single = range::Gaussian(2, -59.444, -59.444, eps);
        EXPECT(single.size() == 1);

        EXPECT_APPROX(single.values().front(), ref.back(), eps);
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
