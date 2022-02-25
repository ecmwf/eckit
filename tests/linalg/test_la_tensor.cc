/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <numeric>

#include "eckit/config/Resource.h"
#include "eckit/io/AutoCloser.h"
#include "eckit/serialisation/FileStream.h"
#include "util.h"

using eckit::linalg::TensorDouble;
using eckit::linalg::TensorFloat;

namespace eckit {
namespace test {


//----------------------------------------------------------------------------------------------------------------------


CASE("TensorDouble [2, 2]") {
    TensorDouble A = TD({2, 2}, 1., -2., -4., 3.);

    // column-major order
    EXPECT(A(0, 0) == 1.);
    EXPECT(A(1, 0) == -2.);
    EXPECT(A(0, 1) == -4.);
    EXPECT(A(1, 1) == 3.);
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
    std::vector<linalg::Size> shape{2, 2, 3, 5};
    std::vector<double> v(TensorDouble::flatSize(shape));

    for (size_t i = 0; i < v.size(); ++i) {
        v[i] = double(i + 1);
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
    auto* data = array.data();
    TensorDouble A{data, {2, 2, 3}};

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
    std::vector<linalg::Size> shape{2, 2, 3, 5};
    std::vector<float> v(TensorFloat::flatSize(shape));

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
    auto* data = array.data();
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


CASE("TensorFloat [2, 3] right to left layout") {
    std::vector<float> array{1., 2., 3., 4., 5., 6.};
    TensorFloat A{array.data(), {2, 3}};
    TensorFloat A_left, A_right;

    // column-major order
    EXPECT(A(0, 0) == 1.);
    EXPECT(A(1, 0) == 2.);
    EXPECT(A(0, 1) == 3.);
    EXPECT(A(1, 1) == 4.);
    EXPECT(A(0, 2) == 5.);
    EXPECT(A(1, 2) == 6.);

    // ** to left layout **
    A_left = A.transformRightToLeftLayout();

    // A internal order should not change!
    for (linalg::Size i = 0; i < A.size(); i++) {
        EXPECT(*(A.data() + i) == array[i]);
    }

    // and no change in the
    // retrieved values
    EXPECT(A(0, 0) == 1.);
    EXPECT(A(1, 0) == 2.);
    EXPECT(A(0, 1) == 3.);
    EXPECT(A(1, 1) == 4.);
    EXPECT(A(0, 2) == 5.);
    EXPECT(A(1, 2) == 6.);

    // internal order is now changed!
    std::vector<float> expected_rowmajor_values{1., 3., 5., 2., 4., 6.};
    for (linalg::Size i = 0; i < A_left.size(); i++) {
        EXPECT(*(A_left.data() + i) == expected_rowmajor_values[i]);
    }

    // ** to right layout **
    A_right = A_left.transformLeftToRightLayout();

    // back to the original order
    EXPECT(A_right(0, 0) == 1.);
    EXPECT(A_right(1, 0) == 2.);
    EXPECT(A_right(0, 1) == 3.);
    EXPECT(A_right(1, 1) == 4.);
    EXPECT(A_right(0, 2) == 5.);
    EXPECT(A_right(1, 2) == 6.);
}


CASE("TensorFloat [2, 3] right->left->right layout") {
    std::vector<float> array{1., 2., 3., 4., 5., 6.};
    TensorFloat A{array.data(), {2, 3}};
    TensorFloat A_left, A_right;

    // A internal order
    for (linalg::Size i = 0; i < A.size(); i++) {
        EXPECT(*(A.data() + i) == array[i]);
    }

    // column-major order
    EXPECT(A(0, 0) == 1.);
    EXPECT(A(1, 0) == 2.);
    EXPECT(A(0, 1) == 3.);
    EXPECT(A(1, 1) == 4.);
    EXPECT(A(0, 2) == 5.);
    EXPECT(A(1, 2) == 6.);

    // ** to left layout **
    A_left = A.transformRightToLeftLayout();

    // internal order is now changed!
    std::vector<float> expected_rowmajor_values{1., 3., 5., 2., 4., 6.};
    for (linalg::Size i = 0; i < A_left.size(); i++) {
        EXPECT(*(A_left.data() + i) == expected_rowmajor_values[i]);
    }

    // indexes still OK
    EXPECT(A(0, 0) == 1.);
    EXPECT(A(1, 0) == 2.);
    EXPECT(A(0, 1) == 3.);
    EXPECT(A(1, 1) == 4.);
    EXPECT(A(0, 2) == 5.);
    EXPECT(A(1, 2) == 6.);

    // ** back to right layout **
    A_right = A_left.transformLeftToRightLayout();

    // check that the internal order is back to initial order
    for (linalg::Size i = 0; i < A_right.size(); i++) {
        EXPECT(*(A_right.data() + i) == array[i]);
    }

    // indexes still OK
    EXPECT(A_right(0, 0) == 1.);
    EXPECT(A_right(1, 0) == 2.);
    EXPECT(A_right(0, 1) == 3.);
    EXPECT(A_right(1, 1) == 4.);
    EXPECT(A_right(0, 2) == 5.);
    EXPECT(A_right(1, 2) == 6.);
}


CASE("TensorFloat [2, 3, 4] right->left->right layout") {
    std::vector<float> array(24);
    for (int i = 0; i < 24; i++) {
        array[i] = i;
    }

    TensorFloat A{array.data(), {2, 3, 4}};
    TensorFloat A_left, A_right;


    // A internal order
    for (linalg::Size i = 0; i < A.size(); i++) {
        EXPECT(*(A.data() + i) == array[i]);
    }

    // indexes
    EXPECT(A(0, 0, 0) == 0.);
    EXPECT(A(1, 0, 0) == 1.);
    EXPECT(A(0, 1, 0) == 2.);
    EXPECT(A(1, 1, 0) == 3.);
    EXPECT(A(0, 2, 0) == 4.);
    EXPECT(A(1, 2, 0) == 5.);

    EXPECT(A(0, 0, 1) == 6.);
    EXPECT(A(1, 0, 1) == 7.);
    EXPECT(A(0, 1, 1) == 8.);
    EXPECT(A(1, 1, 1) == 9.);
    EXPECT(A(0, 2, 1) == 10.);
    EXPECT(A(1, 2, 1) == 11.);

    EXPECT(A(0, 0, 2) == 12.);
    EXPECT(A(1, 0, 2) == 13.);
    EXPECT(A(0, 1, 2) == 14.);
    EXPECT(A(1, 1, 2) == 15.);
    EXPECT(A(0, 2, 2) == 16.);
    EXPECT(A(1, 2, 2) == 17.);

    EXPECT(A(0, 0, 3) == 18.);
    EXPECT(A(1, 0, 3) == 19.);
    EXPECT(A(0, 1, 3) == 20.);
    EXPECT(A(1, 1, 3) == 21.);
    EXPECT(A(0, 2, 3) == 22.);
    EXPECT(A(1, 2, 3) == 23.);

    // ** to left layout **
    A_left = A.transformRightToLeftLayout();

    // internal order is now changed!
    std::vector<float> expected_rowmajor_values{0, 6, 12, 18, 2, 8, 14, 20, 4, 10, 16, 22,
                                                1, 7, 13, 19, 3, 9, 15, 21, 5, 11, 17, 23};
    for (linalg::Size i = 0; i < A_left.size(); i++) {
        EXPECT(*(A_left.data() + i) == expected_rowmajor_values[i]);
    }

    // indexes still OK
    EXPECT(A(0, 0, 0) == 0.);
    EXPECT(A(1, 0, 0) == 1.);
    EXPECT(A(0, 1, 0) == 2.);
    EXPECT(A(1, 1, 0) == 3.);
    EXPECT(A(0, 2, 0) == 4.);
    EXPECT(A(1, 2, 0) == 5.);

    EXPECT(A(0, 0, 1) == 6.);
    EXPECT(A(1, 0, 1) == 7.);
    EXPECT(A(0, 1, 1) == 8.);
    EXPECT(A(1, 1, 1) == 9.);
    EXPECT(A(0, 2, 1) == 10.);
    EXPECT(A(1, 2, 1) == 11.);

    EXPECT(A(0, 0, 2) == 12.);
    EXPECT(A(1, 0, 2) == 13.);
    EXPECT(A(0, 1, 2) == 14.);
    EXPECT(A(1, 1, 2) == 15.);
    EXPECT(A(0, 2, 2) == 16.);
    EXPECT(A(1, 2, 2) == 17.);

    EXPECT(A(0, 0, 3) == 18.);
    EXPECT(A(1, 0, 3) == 19.);
    EXPECT(A(0, 1, 3) == 20.);
    EXPECT(A(1, 1, 3) == 21.);
    EXPECT(A(0, 2, 3) == 22.);
    EXPECT(A(1, 2, 3) == 23.);

    // ** back to right layout **
    A_right = A_left.transformLeftToRightLayout();

    // check that the internal order is back to initial order
    for (linalg::Size i = 0; i < A_right.size(); i++) {
        EXPECT(*(A_right.data() + i) == array[i]);
    }
}


CASE("TensorFloat [6, 3, 2, 5] right->left->right layout") {
    std::vector<float> array(6 * 3 * 2 * 5);
    for (size_t i = 0; i < array.size(); i++) {
        array[i] = i;
    }

    // tensor A (right-layout by default)
    TensorFloat A{array.data(), {6, 3, 2, 5}};
    TensorFloat A_left, A_right;

    // check A internal order
    for (linalg::Size i = 0; i < A.size(); i++) {
        EXPECT(*(A.data() + i) == array[i]);
    }

    // ** to left layout **
    A_left = A.transformRightToLeftLayout();

    // internal order is now changed!
    std::vector<float> expected_rowmajor_values{0, 36, 72, 108, 144, 18, 54, 90, 126, 162, 6, 42, 78,
                                                114, 150, 24, 60, 96, 132, 168, 12, 48, 84, 120, 156,
                                                30, 66, 102, 138, 174, 1, 37, 73, 109, 145, 19, 55, 91,
                                                127, 163, 7, 43, 79, 115, 151, 25, 61, 97, 133, 169, 13,
                                                49, 85, 121, 157, 31, 67, 103, 139, 175, 2, 38, 74, 110,
                                                146, 20, 56, 92, 128, 164, 8, 44, 80, 116, 152, 26, 62, 98,
                                                134, 170, 14, 50, 86, 122, 158, 32, 68, 104, 140, 176, 3, 39,
                                                75, 111, 147, 21, 57, 93, 129, 165, 9, 45, 81, 117, 153, 27,
                                                63, 99, 135, 171, 15, 51, 87, 123, 159, 33, 69, 105, 141,
                                                177, 4, 40, 76, 112, 148, 22, 58, 94, 130, 166, 10, 46, 82,
                                                118, 154, 28, 64, 100, 136, 172, 16, 52, 88, 124, 160, 34,
                                                70, 106, 142, 178, 5, 41, 77, 113, 149, 23, 59, 95, 131, 167,
                                                11, 47, 83, 119, 155, 29, 65, 101, 137, 173, 17, 53, 89, 125,
                                                161, 35, 71, 107, 143, 179};
    for (linalg::Size i = 0; i < A_left.size(); i++) {
        EXPECT(*(A_left.data() + i) == expected_rowmajor_values[i]);
    }

    // ** back to right layout **
    A_right = A_left.transformLeftToRightLayout();

    // check that the internal order is back to initial order
    for (linalg::Size i = 0; i < A_right.size(); i++) {
        EXPECT(*(A_right.data() + i) == array[i]);
    }
}

CASE("TensorFloat move constructor") {

    std::vector<float> data{0,1,2,3,4,5,6,7,8,9,10,11};
    std::vector<linalg::Size> shape{3,4};

    TensorFloat tensor_from(data.data(), shape, false);

    // force move constructor
    TensorFloat tensor_to(std::move(tensor_from));

    EXPECT(tensor_to.shape()[0] == 3);
    EXPECT(tensor_to.shape()[1] == 4);

    EXPECT(tensor_to(0,0) == 0);
    EXPECT(tensor_to(0,1) == 1);
    EXPECT(tensor_to(0,2) == 2);
    EXPECT(tensor_to(0,3) == 3);

    EXPECT(tensor_to(0,4) == 4);
    EXPECT(tensor_to(0,5) == 5);
    EXPECT(tensor_to(0,6) == 6);
    EXPECT(tensor_to(0,7) == 7);

    EXPECT(tensor_to(0,8) == 8);
    EXPECT(tensor_to(0,9) == 9);
    EXPECT(tensor_to(0,10) == 10);
    EXPECT(tensor_to(0,11) == 11);

    EXPECT(tensor_from.data() == nullptr);
}

CASE("TensorFloat move assignment operator") {

    std::vector<float> data{0,1,2,3,4,5,6,7,8,9,10,11};
    std::vector<linalg::Size> shape{3,4};

    TensorFloat tensor_from(data.data(), shape, false);

    // force move assignment
    TensorFloat tensor_to;
    tensor_to = std::move(tensor_from);

    EXPECT(tensor_to.shape()[0] == 3);
    EXPECT(tensor_to.shape()[1] == 4);

    EXPECT(tensor_to(0,0) == 0);
    EXPECT(tensor_to(0,1) == 1);
    EXPECT(tensor_to(0,2) == 2);
    EXPECT(tensor_to(0,3) == 3);

    EXPECT(tensor_to(0,4) == 4);
    EXPECT(tensor_to(0,5) == 5);
    EXPECT(tensor_to(0,6) == 6);
    EXPECT(tensor_to(0,7) == 7);

    EXPECT(tensor_to(0,8) == 8);
    EXPECT(tensor_to(0,9) == 9);
    EXPECT(tensor_to(0,10) == 10);
    EXPECT(tensor_to(0,11) == 11);

    EXPECT(tensor_from.data() == nullptr);

}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

int main(int argc, char** argv) {
    eckit::Main::initialise(argc, argv);
    return eckit::testing::run_tests(argc, argv, false);
}
