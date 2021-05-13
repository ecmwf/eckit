/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <functional>
#include <numeric>
#include <string>

#include "eckit/config/Resource.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/runtime/Main.h"

#include "./util.h"
#include "eckit/linalg/Tensor.h"
#include "eckit/log/Log.h"
#include "eckit/testing/Test.h"

using namespace eckit::linalg;

namespace eckit {
namespace test {


CASE("Tensor [2, 2]") {
    Tensor A = T({2, 2}, 1., -2., -4., 3.);
    // EXPECT(T(0, 0) ==  1.);
    // EXPECT(T(1, 0) == -2.);
    // EXPECT(T(0, 1) == -4.);
    // EXPECT(T(1, 1) ==  3.);
}


CASE("Tensor [2, 3, 4]") {
    Tensor A = T({2, 2, 3}, 1., -2., 3., -4., 5., -6., 7., -8., 9., -10., 11., -12.);
    // EXPECT(T(0, 0) ==  1.);
    // EXPECT(T(1, 0) == -2.);
    // EXPECT(T(0, 1) == -4.);
    // EXPECT(T(1, 1) ==  3.);
}


CASE("Wrap const data") {
    using namespace linalg;
    std::vector<double> array{1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12.};
    const double* data = array.data();
    Tensor T{data, {2, 2}};

    // Tensor access is column-major!!!
    // EXPECT(T(0, 0) ==  1.);
    // EXPECT(T(1, 0) ==  2.);
    // EXPECT(T(0, 1) ==  3.);
    // EXPECT(T(1, 1) ==  4.);
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    using namespace eckit;
    return testing::run_tests(argc, argv, false);
}
