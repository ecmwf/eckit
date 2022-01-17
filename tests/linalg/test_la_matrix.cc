/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "util.h"

namespace eckit {
namespace test {

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

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    eckit::Main::initialise(argc, argv);

    return eckit::testing::run_tests(argc, argv, false);
}
