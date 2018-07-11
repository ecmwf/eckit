/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef eckit_test_linalg_util_h
#define eckit_test_linalg_util_h

#include <cstdarg>

#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/Vector.h"

namespace eckit {
namespace linalg {

//----------------------------------------------------------------------------------------------------------------------

Vector V(Size s, ...) {
    Vector vec(s);
    va_list args;
    va_start(args, s);
    for (Size i = 0; i < s; ++i) vec[i] = va_arg(args, Scalar);
    va_end(args);
    return vec;
}


Matrix M(Matrix::Size rows, Matrix::Size cols, ...) {
    Matrix mat(rows, cols);
    va_list args;
    va_start(args, cols);
    for (Matrix::Size r = 0; r < rows; ++r)
        for (Matrix::Size c = 0; c < cols; ++c)
            mat(r, c) = va_arg(args, Scalar);
    va_end(args);
    return mat;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace linalg
}  // namespace eckit

#endif
