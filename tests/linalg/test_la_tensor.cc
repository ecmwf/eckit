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
#include "eckit/linalg/Tensor.h"
#include "eckit/log/Log.h"
#include "eckit/io/AutoCloser.h"
#include "eckit/runtime/Main.h"
#include "eckit/serialisation/FileStream.h"

#include "eckit/testing/Test.h"


#include "./util.h"

using namespace eckit::linalg;
using namespace eckit::testing;

namespace eckit {
namespace test {


CASE("Tensor [2, 2]") {
    Tensor A = T({2, 2}, 1., -2., -4., 3.);

    // Tensor access is column-major
    EXPECT(A(0, 0) == 1.);
    EXPECT(A(1, 0) == -2.);
    EXPECT(A(0, 1) == -4.);
    EXPECT(A(1, 1) ==  3.);
}


CASE("Tensor [2, 3, 4]") {
    Tensor A = T({2, 2, 3}, 1., -2., 3., -4., 5., -6., 7., -8., 9., -10., 11., -12.);

    // Tensor access is column-major
    EXPECT(A(0, 0, 0) == 1.);
    EXPECT(A(1, 0, 0) == -2.);
    EXPECT(A(0, 1, 0) == 3.);
    EXPECT(A(1, 1, 0) == -4.);
    EXPECT(A(1, 1, 2) == -12.);
}


CASE("Tensor [2, 3, 4, 5]") {
    std::vector<Size> shape {2, 2, 3, 5};
    std::vector<double> v(Tensor::flatten(shape));

    for (size_t i = 0; i < v.size(); ++i) {
        v[i] = double(i+1);
    }

    Tensor A(v.data(), shape);

    // Tensor access is column-major
    EXPECT(A(0, 0, 0, 0) == 1.);
    EXPECT(A(1, 0, 0, 0) == 2.);
    EXPECT(A(0, 1, 0, 0) == 3.);
    EXPECT(A(1, 1, 0, 0) == 4.);

    EXPECT(A(1, 1, 2, 0) == 12.);

    EXPECT(A(0, 0, 2, 4) == 57.);
    EXPECT(A(1, 0, 2, 4) == 58.);
    EXPECT(A(0, 1, 2, 4) == 59.);
    EXPECT(A(1, 1, 2, 4) == 60.);
}


CASE("Tensor serialization") {

    Tensor A = T({2, 2, 3}, 1., -2., 3., -4., 5., -6., 7., -8., 9., -10., 11., -12.);

    PathName path("tensorA");
    {
        FileStream s(path, "w");
        auto c = closer(s);
        s << A;
    }

    FileStream s(path, "r");
    auto c = closer(s);
    Tensor B(s);

    // Tensor access is column-major
    EXPECT(B(0, 0, 0) == 1.);
    EXPECT(B(1, 0, 0) == -2.);
    EXPECT(B(0, 1, 0) == 3.);
    EXPECT(B(1, 1, 0) == -4.);
    EXPECT(B(1, 1, 2) == -12.);

    path.unlink();
}

CASE("Tensor zero") {
    Tensor A = T({2, 2, 3}, 1., -2., 3., -4., 5., -6., 7., -8., 9., -10., 11., -12.);

    // Tensor access is column-major
    EXPECT(A(0, 0, 0) == 1.);
    EXPECT(A(1, 0, 0) == -2.);
    EXPECT(A(0, 1, 0) == 3.);
    EXPECT(A(1, 1, 0) == -4.);
    EXPECT(A(1, 1, 2) == -12.);

    A.zero();

    EXPECT(A(1, 0, 0) == 0.);
    EXPECT(A(0, 1, 0) == 0.);
    EXPECT(A(1, 1, 0) == 0.);
    EXPECT(A(1, 1, 2) == 0.);
}


CASE("Fill Tensor with scalar") {
    Tensor A = T({2, 2, 3}, 1., -2., 3., -4., 5., -6., 7., -8., 9., -10., 11., -12.);

    // Tensor access is column-major
    EXPECT(A(0, 0, 0) == 1.);
    EXPECT(A(1, 0, 0) == -2.);
    EXPECT(A(0, 1, 0) == 3.);
    EXPECT(A(1, 1, 0) == -4.);
    EXPECT(A(1, 1, 2) == -12.);

    A.fill(-7.42);

    EXPECT(A(1, 0, 0) == -7.42);
    EXPECT(A(0, 1, 0) == -7.42);
    EXPECT(A(1, 1, 0) == -7.42);
    EXPECT(A(0, 1, 2) == -7.42);
    EXPECT(A(1, 1, 2) == -7.42);
}


CASE("Fill Tensor with scalar") {
    Tensor A = T({2, 2, 3}, 1., -2., 3., -4., 5., -6., 7., -8., 9., -10., 11., -12.);

    // Tensor access is column-major
    EXPECT(A(0, 0, 0) == 1.);
    EXPECT(A(1, 0, 0) == -2.);
    EXPECT(A(0, 1, 0) == 3.);
    EXPECT(A(1, 1, 0) == -4.);
    EXPECT(A(1, 1, 2) == -12.);

    A.fill(-7.42);

    EXPECT(A(1, 0, 0) == -7.42);
    EXPECT(A(0, 1, 0) == -7.42);
    EXPECT(A(1, 1, 0) == -7.42);
    EXPECT(A(0, 1, 2) == -7.42);
    EXPECT(A(1, 1, 2) == -7.42);
}


CASE("Wrap const data") {
    std::vector<double> array{1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12.};
    const double* data = array.data();
    Tensor A{data, {2, 2, 3}};

    // // Tensor access is column-major
    EXPECT(A(0, 0, 0) == 1.);
    EXPECT(A(1, 0, 0) == 2.);
    EXPECT(A(0, 1, 0) == 3.);
    EXPECT(A(1, 1, 0) == 4.);
    EXPECT(A(1, 1, 1) == 8.);
    EXPECT(A(0, 0, 2) == 9.);
    EXPECT(A(1, 1, 2) == 12.);
}


//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    eckit::Main::initialise(argc, argv);
    return run_tests(argc, argv, false);
}
