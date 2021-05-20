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


//----------------------------------------------------------------------------------------------------------------------


CASE("TensorDouble [2, 2]") {
    TensorDouble A = TD({2, 2}, 1., -2., -4., 3.);

    // column-major order
    EXPECT(A(0, 0) == 1.);
    EXPECT(A(1, 0) == -2.);
    EXPECT(A(0, 1) == -4.);
    EXPECT(A(1, 1) ==  3.);
}


CASE("TensorDouble [2, 3, 4]") {
    TensorDouble A = TD({2, 2, 3}, 1., -2., 3., -4., 5., -6., 7., -8., 9., -10., 11., -12.);

    // column-major order
    EXPECT(A(0, 0, 0) == 1.);
    EXPECT(A(1, 0, 0) == -2.);
    EXPECT(A(0, 1, 0) == 3.);
    EXPECT(A(1, 1, 0) == -4.);
    EXPECT(A(1, 1, 2) == -12.);
}


CASE("TensorDouble [2, 3, 4, 5]") {
    std::vector<Size> shape {2, 2, 3, 5};
    std::vector<double> v(TensorDouble::flatten(shape));

    for (size_t i = 0; i < v.size(); ++i) {
        v[i] = double(i+1);
    }

    TensorDouble A(v.data(), shape);

    // column-major order
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


CASE("TensorDouble serialization") {

    TensorDouble A = TD({2, 2, 3}, 1., -2., 3., -4., 5., -6., 7., -8., 9., -10., 11., -12.);

    PathName path("tensorA");
    {
        FileStream s(path, "w");
        auto c = closer(s);
        s << A;
    }

    FileStream s(path, "r");
    auto c = closer(s);
    TensorDouble B(s);

    // column-major order
    EXPECT(B(0, 0, 0) == 1.);
    EXPECT(B(1, 0, 0) == -2.);
    EXPECT(B(0, 1, 0) == 3.);
    EXPECT(B(1, 1, 0) == -4.);
    EXPECT(B(1, 1, 2) == -12.);

    path.unlink();
}

CASE("TensorDouble zero") {
    TensorDouble A = TD({2, 2, 3}, 1., -2., 3., -4., 5., -6., 7., -8., 9., -10., 11., -12.);

    // column-major order
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


CASE("Fill TensorDouble with scalar") {
    TensorDouble A = TD({2, 2, 3}, 1., -2., 3., -4., 5., -6., 7., -8., 9., -10., 11., -12.);

    // column-major order
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


CASE("Fill TensorDouble with scalar") {
    TensorDouble A = TD({2, 2, 3}, 1., -2., 3., -4., 5., -6., 7., -8., 9., -10., 11., -12.);

    // column-major order
    EXPECT(A(0, 0, 0) == 1.);
    EXPECT(A(1, 0, 0) == -2.);
    EXPECT(A(0, 1, 0) == 3.);
    EXPECT(A(1, 1, 0) == -4.);
    EXPECT(A(1, 1, 2) == -12.);

    double v = -7.42;
    A.fill(v);

    EXPECT(A(1, 0, 0) == v);
    EXPECT(A(0, 1, 0) == v);
    EXPECT(A(1, 1, 0) == v);
    EXPECT(A(0, 1, 2) == v);
    EXPECT(A(1, 1, 2) == v);
}


CASE("TensorDouble wrapping const data") {
    std::vector<double> array{1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12.};
    const double* data = array.data();
    TensorDouble A {data, {2, 2, 3}};

    // // column-major order
    EXPECT(A(0, 0, 0) == 1.);
    EXPECT(A(1, 0, 0) == 2.);
    EXPECT(A(0, 1, 0) == 3.);
    EXPECT(A(1, 1, 0) == 4.);
    EXPECT(A(1, 1, 1) == 8.);
    EXPECT(A(0, 0, 2) == 9.);
    EXPECT(A(1, 1, 2) == 12.);
}

//----------------------------------------------------------------------------------------------------------------------

CASE("TensorFloat [2, 2]") {
    TensorFloat A = TF({2, 2}, 1., -2., -4., 3.);

    // column-major order
    EXPECT(A(0, 0) == 1.);
    EXPECT(A(1, 0) == -2.);
    EXPECT(A(0, 1) == -4.);
    EXPECT(A(1, 1) == 3.);
}


CASE("TensorFloat [2, 3, 4]") {
    TensorFloat A = TF({2, 2, 3}, 1., -2., 3., -4., 5., -6., 7., -8., 9., -10., 11., -12.);

    // column-major order
    EXPECT(A(0, 0, 0) == 1.);
    EXPECT(A(1, 0, 0) == -2.);
    EXPECT(A(0, 1, 0) == 3.);
    EXPECT(A(1, 1, 0) == -4.);
    EXPECT(A(1, 1, 2) == -12.);
}


CASE("TensorFloat [2, 3, 4, 5]") {
    std::vector<Size> shape{2, 2, 3, 5};
    std::vector<float> v(TensorFloat::flatten(shape));

    for (size_t i = 0; i < v.size(); ++i) {
        v[i] = float(i + 1);
    }

    TensorFloat A(v.data(), shape);

    // column-major order
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


CASE("TensorFloat serialization") {

    TensorFloat A = TF({2, 2, 3}, 1., -2., 3., -4., 5., -6., 7., -8., 9., -10., 11., -12.);

    PathName path("tensorA");
    {
        FileStream s(path, "w");
        auto c = closer(s);
        s << A;
    }

    FileStream s(path, "r");
    auto c = closer(s);
    TensorFloat B(s);

    // column-major order
    EXPECT(B(0, 0, 0) == 1.);
    EXPECT(B(1, 0, 0) == -2.);
    EXPECT(B(0, 1, 0) == 3.);
    EXPECT(B(1, 1, 0) == -4.);
    EXPECT(B(1, 1, 2) == -12.);

    path.unlink();
}

CASE("TensorFloat zero") {
    TensorFloat A = TF({2, 2, 3}, 1., -2., 3., -4., 5., -6., 7., -8., 9., -10., 11., -12.);

    // column-major order
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


CASE("Fill TensorFloat with scalar") {
    TensorFloat A = TF({2, 2, 3}, 1., -2., 3., -4., 5., -6., 7., -8., 9., -10., 11., -12.);

    // column-major order
    EXPECT(A(0, 0, 0) == 1.);
    EXPECT(A(1, 0, 0) == -2.);
    EXPECT(A(0, 1, 0) == 3.);
    EXPECT(A(1, 1, 0) == -4.);
    EXPECT(A(1, 1, 2) == -12.);

    float v = -7.42;
    A.fill(v);

    EXPECT(A(1, 0, 0) == v);
    EXPECT(A(0, 1, 0) == v);
    EXPECT(A(1, 1, 0) == v);
    EXPECT(A(0, 1, 2) == v);
    EXPECT(A(1, 1, 2) == v);
}


CASE("TensorFloat wrapping const data") {
    std::vector<float> array{1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12.};
    const float* data = array.data();
    TensorFloat A{data, {2, 2, 3}};

    // // column-major order
    EXPECT(A(0, 0, 0) == 1.);
    EXPECT(A(1, 0, 0) == 2.);
    EXPECT(A(0, 1, 0) == 3.);
    EXPECT(A(1, 1, 0) == 4.);
    EXPECT(A(1, 1, 1) == 8.);
    EXPECT(A(0, 0, 2) == 9.);
    EXPECT(A(1, 1, 2) == 12.);
}


CASE("TensorFloat [2, 3] right layout") {

    std::vector<float> array{1., 2., 3., 4., 5., 6.};
    TensorFloat A{array.data(), {2, 3}};

    // column-major order
    EXPECT(A(0, 0) == 1.);
    EXPECT(A(1, 0) == 2.);
    EXPECT(A(0, 1) == 3.);
    EXPECT(A(1, 1) == 4.);
    EXPECT(A(0, 2) == 5.);
    EXPECT(A(1, 2) == 6.);

    // to right layout
    A.toRightLayout();

    // A internal order should not change!
    for (int i=0; i<A.size(); i++)
        EXPECT( *(A.data()+i) == array[i]);

    // and no change in the
    // retrieved values
    EXPECT(A(0, 0) == 1.);
    EXPECT(A(1, 0) == 2.);
    EXPECT(A(0, 1) == 3.);
    EXPECT(A(1, 1) == 4.);
    EXPECT(A(0, 2) == 5.);
    EXPECT(A(1, 2) == 6.);

    // ** to left layout **
    A.toLeftLayout();

    // internal order is now changed!
    std::vector<float> expected_rowmajor_values{1., 3., 5., 2., 4., 6.};
    for (int i=0; i<A.size(); i++)
        EXPECT( *(A.data()+i) == expected_rowmajor_values[i]);

    // the outside indexes should still
    // give back the correct values..
    EXPECT(A(0, 0) == 1.);
    EXPECT(A(1, 0) == 2.);
    EXPECT(A(0, 1) == 3.);
    EXPECT(A(1, 1) == 4.);
    EXPECT(A(0, 2) == 5.);
    EXPECT(A(1, 2) == 6.);

}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    eckit::Main::initialise(argc, argv);
    return run_tests(argc, argv, false);
}
