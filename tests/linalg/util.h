/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef eckit_test_la_util_h
#define eckit_test_la_util_h

#include <cstdarg>

#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/Vector.h"
#include "eckit/linalg/Tensor.h"

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

linalg::Tensor T(std::vector<linalg::Size> shape, ...) {
    using linalg::Tensor;
    using linalg::Scalar;
    using linalg::Size;

    Size size = eckit::linalg::Tensor::flatten(shape);
    Tensor t(shape);
    va_list args;
    va_start(args, shape);
    for (Size i = 0; i < size; ++i) {
        t[i] = va_arg(args, Scalar);
    }
    va_end(args);
    return t;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit

#endif
