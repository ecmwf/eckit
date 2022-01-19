/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/sparse/LinearAlgebraGeneric.h"

#include <ostream>
#include <vector>

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"


namespace eckit {
namespace linalg {
namespace sparse {


static const LinearAlgebraGeneric __la_generic("generic");
#ifdef eckit_HAVE_OMP
static const LinearAlgebraGeneric __la_openmp("openmp");
#endif


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
#pragma omp parallel
#endif
    {
        std::vector<Scalar> sum(Nk);

#ifdef eckit_HAVE_OMP
#pragma omp for
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


}  // namespace sparse
}  // namespace linalg
}  // namespace eckit
