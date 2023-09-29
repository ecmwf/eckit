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


#define EXPECT_APPROX(a, b) EXPECT(::eckit::types::is_approximately_equal((a), (b), 1e-12))


namespace eckit::test {


CASE("range::Gaussian") {
    using geo::range::Gaussian;

    std::vector<double> ref{59.44440828916676, 19.87571914744090, -19.87571914744090, -59.44440828916676};

    auto global = Gaussian(2);

    EXPECT(global.size() == ref.size());

    size_t i = 0;
    for (const auto& test : global.values()) {
        EXPECT_APPROX(test, ref[i++]);
    }

    auto cropped = Gaussian(2, 50., -50., 1e-3);
    EXPECT(cropped.size() == ref.size() - 2);

    i = 1;
    for (const auto& test : cropped.values()) {
        EXPECT_APPROX(test, ref[i++]);
    }

    EXPECT(Gaussian(2, 59.444, -59.444, 1e-3).size() == 4);
    EXPECT(Gaussian(2, 59.444, -59.444, 1e-6).size() == 2);
    EXPECT(Gaussian(2, 59.444, -59.445, 1e-6).size() == 3);

    auto single = Gaussian(2, -59.444, -59.444, 1e-3);
    EXPECT(single.size() == 1);

    EXPECT_APPROX(single.values().front(), ref.back());
}


}  // namespace eckit::test


int main(int argc, char** argv) {
    return eckit::testing::run_tests(argc, argv);
}
