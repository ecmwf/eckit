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

#include <vector>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"


namespace eckit {
namespace linalg {


namespace {
static const std::string __name{"generic"};

static const dense::LinearAlgebraGeneric __lad_generic(__name);
static const sparse::LinearAlgebraGeneric __las_generic(__name);
static const deprecated::LinearAlgebraGeneric __la_generic(__name);

#ifdef eckit_HAVE_OMP
static const std::string __name_openmp{"openmp"};

static const dense::LinearAlgebraGeneric __lad_openmp(__name_openmp);
static const sparse::LinearAlgebraGeneric __las_openmp(__name_openmp);
static const deprecated::LinearAlgebraGeneric __la_openmp(__name_openmp);
#endif
}  // anonymous namespace


namespace dense {


//-----------------------------------------------------------------------------


void LinearAlgebraGeneric::print(std::ostream& out) const {
    out << "LinearAlgebraGeneric[]";
}


Scalar LinearAlgebraGeneric::dot(const Vector& x, const Vector& y) const {
    const auto Ni = x.size();
    ASSERT(y.size() == Ni);

    Scalar sum = 0.;

#ifdef eckit_HAVE_OMP
#pragma omp parallel for reduction(+ \
                                   : sum)
#endif
    for (Size i = 0; i < Ni; ++i) {
        const auto p = x[i] * y[i];
        sum += p;
    }

    return sum;
}


void LinearAlgebraGeneric::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    const auto Ni = A.rows();
    const auto Nj = A.cols();

    ASSERT(y.rows() == Ni);
    ASSERT(x.rows() == Nj);

#ifdef eckit_HAVE_OMP
#pragma omp parallel for
#endif
    for (Size i = 0; i < Ni; ++i) {
        Scalar sum = 0.;

        for (Size j = 0; j < Nj; ++j) {
            sum += A(i, j) * x[j];
        }

        y[i] = sum;
    }
}


void LinearAlgebraGeneric::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    const auto Ni = A.rows();
    const auto Nj = B.cols();
    const auto Nk = A.cols();

    ASSERT(C.rows() == Ni);
    ASSERT(C.cols() == Nj);
    ASSERT(B.rows() == Nk);

#ifdef eckit_HAVE_OMP
#pragma omp parallel for collapse(2)
#endif
    for (Size j = 0; j < Nj; ++j) {
        for (Size i = 0; i < Ni; ++i) {
            Scalar sum = 0.;

            for (Size k = 0; k < Nk; ++k) {
                sum += A(i, k) * B(k, j);
            }

            C(i, j) = sum;
        }
    }
}


//-----------------------------------------------------------------------------


}  // namespace dense


//-----------------------------------------------------------------------------


namespace sparse {


//-----------------------------------------------------------------------------


void LinearAlgebraGeneric::print(std::ostream& out) const {
    out << "LinearAlgebraGeneric[]";
}


void LinearAlgebraGeneric::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    const auto Ni = A.rows();
    const auto Nj = A.cols();

    ASSERT(y.rows() == Ni);
    ASSERT(x.rows() == Nj);

    if (A.empty()) {
        return;
    }

    const auto* const outer = A.outer();
    const auto* const inner = A.inner();
    const auto* const val   = A.data();

    ASSERT(outer[0] == 0);  // expect indices to be 0-based

#ifdef eckit_HAVE_OMP
#pragma omp parallel for
#endif
    for (Size i = 0; i < Ni; ++i) {
        Scalar sum = 0.;

        for (auto c = outer[i]; c < outer[i + 1]; ++c) {
            sum += val[c] * x[static_cast<Size>(inner[c])];
        }

        y[i] = sum;
    }
}


void LinearAlgebraGeneric::spmm(const SparseMatrix& A, const Matrix& B, Matrix& C) const {
    const auto Ni = A.rows();
    const auto Nj = A.cols();
    const auto Nk = B.cols();

    ASSERT(C.rows() == Ni);
    ASSERT(B.rows() == Nj);
    ASSERT(C.cols() == Nk);

    if (A.empty()) {
        return;
    }

    const auto* const outer = A.outer();
    const auto* const inner = A.inner();
    const auto* const val   = A.data();

    ASSERT(outer[0] == 0);  // expect indices to be 0-based

    std::vector<Scalar> sum;

#ifdef eckit_HAVE_OMP
#pragma omp parallel for private(sum)
#endif
    for (Size i = 0; i < Ni; ++i) {
        sum.assign(Nk, 0);

        for (auto c = outer[i]; c < outer[i + 1]; ++c) {
            const auto j = static_cast<Size>(inner[c]);
            const auto v = val[c];
            for (Size k = 0; k < Nk; ++k) {
                sum[k] += v * B(j, k);
            }
        }

        for (Size k = 0; k < Nk; ++k) {
            C(i, k) = sum[k];
        }
    }
}


void LinearAlgebraGeneric::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    const auto Ni = A.rows();
    const auto Nj = A.cols();

    ASSERT(x.size() == Ni);
    ASSERT(y.size() == Nj);

    B = A;
    if (A.empty()) {
        return;
    }

    const auto* const outer = B.outer();
    const auto* const inner = B.inner();
    auto* const val         = const_cast<Scalar*>(B.data());

    ASSERT(outer[0] == 0);  // expect indices to be 0-based

#ifdef eckit_HAVE_OMP
#pragma omp parallel for
#endif
    for (Size i = 0; i < Ni; ++i) {
        for (auto k = outer[i]; k < outer[i + 1]; ++k) {
            const auto j = static_cast<Size>(inner[k]);
            ASSERT(j < Nj);
            val[k] *= x[i] * y[j];
        }
    }
}


//-----------------------------------------------------------------------------


}  // namespace sparse
}  // namespace linalg
}  // namespace eckit
