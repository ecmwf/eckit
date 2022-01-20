/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/sparse/LinearAlgebraMKL.h"

#include "mkl.h"

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"
#include "eckit/linalg/sparse/LinearAlgebraGeneric.h"


namespace eckit {
namespace linalg {
namespace sparse {


static const LinearAlgebraMKL __la("mkl");

static const double alpha = 1.;
static const double beta  = 0.;


void LinearAlgebraMKL::print(std::ostream& out) const {
    out << "LinearAlgebraMKL[]";
}


void LinearAlgebraMKL::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    ASSERT(x.size() == A.cols());
    ASSERT(y.size() == A.rows());

    // We expect indices to be 0-based
    ASSERT(A.outer()[0] == 0);

    const auto m = static_cast<MKL_INT>(A.rows());
    const auto k = static_cast<MKL_INT>(A.cols());

    // FIXME: mkl_dcsrmv is deprecated, use mkl_sparse_d_mv instead
    // void mkl_dcsrmv(const char *transa, const MKL_INT *m, const MKL_INT *k,
    //                 const double *alpha, const char *matdescra,
    //                 const double *val, const MKL_INT *indx, const MKL_INT *pntrb, const MKL_INT *pntre,
    //                 const double *x, const double *beta, double *y);

    const auto* matrix = static_cast<const double*>(A.data());
    const auto* inner  = static_cast<const MKL_INT*>(A.inner());
    const auto* outer  = static_cast<const MKL_INT*>(A.outer());
    const auto* vector = static_cast<const double*>(x.data());

    mkl_dcsrmv("N", &m, &k, &alpha, "G__C", matrix, inner, outer, outer + 1, vector, &beta, y.data());
}


void LinearAlgebraMKL::spmm(const SparseMatrix& A, const Matrix& B, Matrix& C) const {
    ASSERT(A.cols() == B.rows());
    ASSERT(A.rows() == C.rows());
    ASSERT(B.cols() == C.cols());

    // We expect indices to be 0-based
    ASSERT(A.outer()[0] == 0);

    const auto m = static_cast<const MKL_INT>(A.rows());
    const auto n = static_cast<const MKL_INT>(C.cols());
    const auto k = static_cast<const MKL_INT>(A.cols());

    // FIXME: with 0-based indexing, MKL assumes row-major ordering for B and C
    // We need to use 1-based indexing i.e. offset outer and inner indices by 1

    std::vector<MKL_INT> pntrb(A.rows() + 1);
    for (Size i = 0; i < A.rows() + 1; ++i) {
        pntrb[i] = A.outer()[i] + 1;
    }

    std::vector<MKL_INT> indx(A.nonZeros());
    for (Size i = 0; i < A.nonZeros(); ++i) {
        indx[i] = A.inner()[i] + 1;
    }

    // FIXME: mkl_dcsrmm is deprecated, use mkl_sparse_d_mm instead
    // void mkl_dcsrmm(const char *transa, const MKL_INT *m, const MKL_INT *n, const MKL_INT *k,
    //                 const double *alpha, const char *matdescra,
    //                 const double *val, const MKL_INT *indx, const MKL_INT *pntrb, const MKL_INT *pntre,
    //                 const double *b, const MKL_INT *ldb, const double *beta,
    //                 double *c, const MKL_INT *ldc);

    mkl_dcsrmm("N", &m, &n, &k, &alpha, "G__F", A.data(), indx.data(), pntrb.data(), pntrb.data() + 1, B.data(), &k,
               &beta, C.data(), &m);
}


void LinearAlgebraMKL::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    static const sparse::LinearAlgebraGeneric generic;
    generic.dsptd(x, A, y, B);
}


}  // namespace sparse
}  // namespace linalg
}  // namespace eckit
