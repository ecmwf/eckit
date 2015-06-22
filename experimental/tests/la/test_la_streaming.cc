/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_la_streaming

#include "ecbuild/boost_test_framework.h"

#include <cstdarg>

#include "eckit/filesystem/PathName.h"
#include "eckit/serialisation/FileStream.h"

#include "eckit/la/Matrix.h"
#include "eckit/la/Vector.h"

//-----------------------------------------------------------------------------

using namespace eckit::la;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

template<typename T>
void stream_test(const T& t) {
    PathName filename = PathName::unique( "data" );
    {
        FileStream sout( filename, "w" );
        sout << t;
    }
    {
        FileStream sin( filename, "r" );
        T out(sin);
        BOOST_CHECK_EQUAL_COLLECTIONS(t.begin(), t.end(), out.begin(), out.end());
    }
    if (filename.exists()) filename.unlink();
}

//-----------------------------------------------------------------------------

/// Test linear algebra interface

BOOST_AUTO_TEST_SUITE(test_eckit_la_streaming)

BOOST_AUTO_TEST_CASE(test_stream_vector) {
    stream_test(V(5, 1., 2., 3., 4., 5.));
}

BOOST_AUTO_TEST_CASE(test_stream_matrix) {
    stream_test(M(3, 3, 1., 2., 3., 4., 5., 6., 7., 8., 9.));
}

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

}  // namespace test
}  // namespace eckit
