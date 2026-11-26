// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "util.h"

namespace eckit::test {

//----------------------------------------------------------------------------------------------------------------------

CASE("test wrapping of const data") {
    std::vector<double> array{1., -2., -4., 2.};
    const auto* data = array.data();
    linalg::Matrix A{data, 2, 2};
    linalg::Vector y{data, 2};

    // Matrix access is column-major!!!
    EXPECT(A(0, 0) == 1.);
    EXPECT(A(1, 0) == -2.);
    EXPECT(A(0, 1) == -4.);
    EXPECT(A(1, 1) == 2.);

    EXPECT(y[0] == 1.);
    EXPECT(y[1] == -2.);
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::test

int main(int argc, char** argv) {
    eckit::Main::initialise(argc, argv);

    return eckit::testing::run_tests(argc, argv, false);
}
