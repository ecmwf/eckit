/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#define BOOST_TEST_MODULE test_eckit_la_linalg

#include "ecbuild/boost_test_framework.h"

#include <cstdarg>

#include "eckit/la/LinearAlgebraFactory.h"
#include "eckit/la/Matrix.h"
#include "eckit/la/Vector.h"

//-----------------------------------------------------------------------------

using namespace eckit::la;

namespace eckit {
namespace test {

//-----------------------------------------------------------------------------

Vector v(Vector::Size s, ...) {
    Vector vec(s);
    va_list args;
    va_start(args, s);
    for (Vector::Size i = 0; i < s; ++i) vec[i] = va_arg(args, Scalar);
    va_end(args);
    return vec;
}

Matrix m(Matrix::Size rows, Matrix::Size cols, ...) {
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

struct Fixture {

    Fixture() : x(v(3, 1., 2., 4.)),
                A(m(2, 2, 1., -2., -4., 2.)),
                linalg(LinearAlgebraFactory::get()) {}

    Vector x;
    Matrix A;
    LinearAlgebraBase& linalg;
};

//-----------------------------------------------------------------------------

template <class T>
void test(const T& v, const T& r) {
    BOOST_CHECK_EQUAL_COLLECTIONS(v.data(), v.data() + v.size(), r.data(), r.data() + r.size());
}

//-----------------------------------------------------------------------------

/// Test linear algebra interface

BOOST_FIXTURE_TEST_SUITE(test_eckit_la_linalg, Fixture)

BOOST_AUTO_TEST_CASE(test_dot) {
    BOOST_CHECK_EQUAL(linalg.dot(x, x), 21.);
    BOOST_TEST_MESSAGE("dot of vectors of different sizes should fail");
    BOOST_CHECK_THROW(linalg.dot(x, Vector(2)), AssertionFailed);
}

BOOST_AUTO_TEST_CASE(test_gemv) {
    Vector y(2);
    linalg.gemv(A, v(2, -1., -2.), y);
    test(y, v(2, 3., 0.));
    BOOST_TEST_MESSAGE("gemv of matrix and vector of nonmatching sizes should fail");
    BOOST_CHECK_THROW(linalg.gemv(A, x, y), AssertionFailed);
}

BOOST_AUTO_TEST_CASE(test_gemm) {
    Matrix B(2, 2);
    linalg.gemm(A, A, B);
    test(B, m(2, 2, 9., -6., -12., 12.));
    BOOST_TEST_MESSAGE("gemm of matrices of nonmatching sizes should fail");
    BOOST_CHECK_THROW(linalg.gemm(A, Matrix(1, 2), B), AssertionFailed);
}

BOOST_AUTO_TEST_SUITE_END()

//-----------------------------------------------------------------------------

}  // namespace test
}  // namespace eckittest
