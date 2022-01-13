/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#pragma once

#include <cstdarg>

#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/Tensor.h"
#include "eckit/linalg/Vector.h"
#include "eckit/testing/Test.h"

namespace eckit {
namespace test {

//----------------------------------------------------------------------------------------------------------------------

linalg::Vector V(linalg::Size s, ...) {
    using linalg::Scalar;
    using linalg::Size;
    using linalg::Vector;

    Vector vec(s);
    va_list args;
    va_start(args, s);
    for (Size i = 0; i < s; ++i) {
        vec[i] = va_arg(args, Scalar);
    }
    va_end(args);
    return vec;
}


linalg::Matrix M(linalg::Size rows, linalg::Size cols, ...) {
    using linalg::Matrix;
    using linalg::Scalar;
    using linalg::Size;

    Matrix mat(rows, cols);
    va_list args;
    va_start(args, cols);
    for (Size r = 0; r < rows; ++r) {
        for (Size c = 0; c < cols; ++c) {
            mat(r, c) = va_arg(args, Scalar);
        }
    }
    va_end(args);
    return mat;
}

linalg::TensorDouble TD(std::vector<linalg::Size> shape, ...) {
    using linalg::Scalar;
    using linalg::Size;
    using linalg::TensorDouble;

    Size size = eckit::linalg::TensorDouble::flatten(shape);
    TensorDouble t(shape);
    va_list args;
    va_start(args, shape);
    for (Size i = 0; i < size; ++i) {
        t[i] = va_arg(args, Scalar);
    }
    va_end(args);
    return t;
}

linalg::TensorFloat TF(std::vector<linalg::Size> shape, ...) {
    using linalg::Scalar;
    using linalg::Size;
    using linalg::TensorFloat;

    Size size = eckit::linalg::TensorFloat::flatten(shape);
    TensorFloat t(shape);
    va_list args;
    va_start(args, shape);
    for (Size i = 0; i < size; ++i) {
        t[i] = va_arg(args, Scalar);
    }
    va_end(args);
    return t;
}

//----------------------------------------------------------------------------------------------------------------------

template <class T>
bool equal_dense_matrix(const T& a, const T& b) {
    return testing::make_view(a.data(), a.data() + a.size()) == testing::make_view(b.data(), b.data() + b.size());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit
