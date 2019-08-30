/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/LinearAlgebraGeneric.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"


namespace eckit {
namespace linalg {


//----------------------------------------------------------------------------------------------------------------------


LinearAlgebraGeneric::LinearAlgebraGeneric() : LinearAlgebra("generic") {}


void LinearAlgebraGeneric::print(std::ostream& out) const {
    out << "LinearAlgebraGeneric[]";
}


Scalar LinearAlgebraGeneric::dot(const Vector& x, const Vector& y) const {

    ASSERT(x.size() == y.size());
    Scalar r = 0;
    for (Size i = 0; i < x.size(); ++i) {
        r += x[i] * y[i];
    }
    return r;
}


void LinearAlgebraGeneric::gemv(const Matrix& A, const Vector& x, Vector& y) const {

    ASSERT(x.size() == A.cols() && y.size() == A.rows());

    for (Size r = 0; r < A.rows(); ++r) {
        double sum = 0.;
        for (Size c = 0; c < A.cols(); ++c) {
            sum += A(r, c) * x[c];
        }
        y[r] = sum;
    }
}


void LinearAlgebraGeneric::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {

    ASSERT(A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols());

    C.setZero();
    for (Size c = 0; c < B.cols(); ++c) {
        for (Size r = 0; r < A.rows(); ++r) {
            for (Size k = 0; k < A.cols(); ++k) {
                C(r, c) += A(r, k) * B(k, c);
            }
        }
    }
}


void LinearAlgebraGeneric::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {

    ASSERT(x.size() == A.cols() && y.size() == A.rows());

    ASSERT(A.outer()[0] == 0);  // expect indices to be 0-based

    const Index* outer = A.outer();
    const Index* inner = A.inner();
    const Scalar* val  = A.data();

    for (Size r = 0; r < A.rows(); ++r) {
        double sum = 0.;
        for (Index oi = outer[r]; oi < outer[r + 1]; ++oi) {
            sum += val[oi] * x[static_cast<Size>(inner[oi])];
        }
        y[r] = sum;
    }
}


void LinearAlgebraGeneric::spmm(const SparseMatrix& A, const Matrix& B, Matrix& C) const {

    ASSERT(A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols());

    ASSERT(A.outer()[0] == 0);  // expect indices to be 0-based

    const Index* outer = A.outer();
    const Index* inner = A.inner();
    const Scalar* val  = A.data();

    C.setZero();
    for (Size r = 0; r < A.rows(); ++r) {
        for (Index oi = outer[r]; oi < outer[r + 1]; ++oi) {
            for (Size c = 0; c < B.cols(); ++c) {
                C(r, c) += val[oi] * B(static_cast<Size>(inner[oi]), c);
            }
        }
    }
}


void LinearAlgebraGeneric::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {

    ASSERT(x.size() == A.rows() && A.cols() == y.size());

    ASSERT(A.outer()[0] == 0);  // expect indices to be 0-based

    B = A;

    const Index* outer = B.outer();
    const Index* inner = B.inner();
    Scalar* val        = const_cast<Scalar*>(B.data());

    for (Size r = 0, k = 0; r < B.rows(); ++r) {
        for (Index j = outer[r]; j < outer[r + 1]; ++j, ++k) {
            auto c = static_cast<Size>(inner[j]);
            ASSERT(c < B.cols());
            val[k] *= x[r] * y[c];
        }
    }
}


static LinearAlgebraGeneric linearAlgebraGeneric;


//----------------------------------------------------------------------------------------------------------------------


}  // namespace linalg
}  // namespace eckit
