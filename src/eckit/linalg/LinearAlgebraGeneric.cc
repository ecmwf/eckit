/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

//-----------------------------------------------------------------------------

#include "eckit/linalg/LinearAlgebraGeneric.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace linalg {

namespace {

/// Multiply A by the diagonal matrix d (in vector form)
void dsp(const Vector& d, SparseMatrix& A) {
    const Index* outer = A.outer();
    // FIXME: better use InnerIterator
    Scalar* data = const_cast<Scalar*>(A.data());
    for (size_t r = 0; r < A.rows(); ++r)
        for (Index oi = outer[r]; oi < outer[r+1]; ++oi)
            data[oi] *= d[r];
}

}

//-----------------------------------------------------------------------------

LinearAlgebraGeneric::LinearAlgebraGeneric() : LinearAlgebra("generic") {}

//-----------------------------------------------------------------------------

void LinearAlgebraGeneric::print(std::ostream& out) const {
    out << "LinearAlgebraGeneric[]";
}

//-----------------------------------------------------------------------------

Scalar LinearAlgebraGeneric::dot(const Vector& x, const Vector& y) const {
    ASSERT(x.size() == y.size());
    Scalar r = 0;
    for (size_t i = 0; i < x.size(); ++i) r += x[i] * y[i];
    return r;
}

//-----------------------------------------------------------------------------

void LinearAlgebraGeneric::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT( x.size() == A.cols() && y.size() == A.rows() );
    for (size_t r = 0; r < A.rows(); ++r) {
        y[r] = 0.;
        for (size_t c = 0; c < A.cols(); ++c) y[r] += A(r, c) * x[c];
    }
}

//-----------------------------------------------------------------------------

void LinearAlgebraGeneric::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    ASSERT( A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols() );
    for (size_t c = 0; c < B.cols(); ++c)
        for (size_t r = 0; r < A.rows(); ++r)
            for (size_t k = 0; k < A.cols(); ++k) C(r, c) += A(r, k) * B(k, c);
}

//-----------------------------------------------------------------------------

void LinearAlgebraGeneric::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    ASSERT( x.size() == A.cols() && y.size() == A.rows() );
    // We expect indices to be 0-based
    ASSERT( A.outer()[0] == 0 );
    const Index* outer = A.outer();
    const Index* inner = A.inner();
    const Scalar* val = A.data();
    std::cout << "Calling Generic::spmv()" << std::endl;
    for (size_t r = 0; r < A.rows(); ++r) {
        y[r] = 0.;
        for (Index oi = outer[r]; oi < outer[r+1]; ++oi)
            y[r] += val[oi] * x[inner[oi]];
    }
}

//-----------------------------------------------------------------------------

void LinearAlgebraGeneric::spmm(const SparseMatrix& A, const Matrix& B, Matrix& C) const {
    ASSERT( A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols() );
    // We expect indices to be 0-based
    ASSERT( A.outer()[0] == 0 );
    const Index* outer = A.outer();
    const Index* inner = A.inner();
    const Scalar* val = A.data();
    for (size_t r = 0; r < A.rows(); ++r)
        for (Index oi = outer[r]; oi < outer[r+1]; ++oi)
            for (size_t c = 0; c < B.cols(); ++c)
                C(r, c) += val[oi] * B(inner[oi], c);
}

//-----------------------------------------------------------------------------

void LinearAlgebraGeneric::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    ASSERT( x.size() == A.cols() && y.size() == A.rows() );
    B = A;
    dsp(y, B);
    B.transpose();
    dsp(x, B);
}

//-----------------------------------------------------------------------------

static LinearAlgebraGeneric linearAlgebraGeneric;

//-----------------------------------------------------------------------------

} // namespace linalg
} // namespace eckit
