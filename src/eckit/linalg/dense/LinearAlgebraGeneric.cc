// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/linalg/dense/LinearAlgebraGeneric.h"

#include <ostream>
#include <vector>

#include "eckit/eckit_config.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/Vector.h"

namespace eckit::linalg::dense {

static const LinearAlgebraGeneric __la_generic("generic");
#if eckit_HAVE_OMP
static const LinearAlgebraGeneric __la_openmp("openmp");
#endif


void LinearAlgebraGeneric::print(std::ostream& out) const {
    out << "LinearAlgebraGeneric[]";
}


Scalar LinearAlgebraGeneric::dot(const Vector& x, const Vector& y) const {
    const auto Ni = x.size();
    ASSERT(y.size() == Ni);

    Scalar sum = 0.;

#if eckit_HAVE_OMP
#pragma omp parallel for reduction(+ : sum)
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

#if eckit_HAVE_OMP
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

#if eckit_HAVE_OMP
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

}  // namespace eckit::linalg::dense
