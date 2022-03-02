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
#include "eckit/linalg/SparseMatrix.h"
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

linalg::SparseMatrix S(linalg::Size rows, linalg::Size cols, linalg::Size nnz, ...) {
    using linalg::Scalar;
    using linalg::Size;
    using linalg::Triplet;

    va_list args;
    va_start(args, nnz);
    std::vector<Triplet> triplets;
    for (Size n = 0; n < nnz; ++n) {
        Size row = Size(va_arg(args, int));
        Size col = Size(va_arg(args, int));
        Scalar v = va_arg(args, Scalar);
        triplets.push_back(Triplet(row, col, v));
    }
    va_end(args);

    linalg::SparseMatrix mat(rows, cols, triplets);

    //    ECKIT_DEBUG_VAR(mat.nonZeros());
    //    ECKIT_DEBUG_VAR(mat.rows());
    //    ECKIT_DEBUG_VAR(mat.cols());

    return mat;
}

linalg::TensorDouble TD(std::vector<linalg::Size> shape, ...) {
    using linalg::Scalar;
    using linalg::Size;
    using linalg::TensorDouble;

    Size size = linalg::TensorDouble::flatSize(shape);
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

    Size size = linalg::TensorFloat::flatSize(shape);
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

template <typename T>
bool equal_array(T* v, T* r, size_t s) {
    return testing::make_view(v, s) == testing::make_view(r, s);
}

template <typename T>
bool equal_dense_matrix(const T& a, const T& b) {
    return a.size() == b.size() && testing::make_view(a.data(), a.data() + a.size()) == testing::make_view(b.data(), b.data() + b.size());
}

template <typename T>
bool equal_dense_matrix(const T& v, const T& r, size_t s) {
    return is_approximately_equal(testing::make_view(v.data(), s), testing::make_view(r.data(), s), 0.1);
}

bool equal_sparse_matrix(const linalg::SparseMatrix& A, const linalg::Index* outer, const linalg::Index* inner, const linalg::Scalar* data) {
    return equal_array(A.outer(), outer, A.rows() + 1) && equal_array(A.inner(), inner, A.nonZeros()) && equal_array(A.data(), data, A.nonZeros());
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit
