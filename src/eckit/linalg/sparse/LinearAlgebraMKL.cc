// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/linalg/sparse/LinearAlgebraMKL.h"

#include <ostream>

#include "mkl.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"
#include "eckit/linalg/sparse/LinearAlgebraGeneric.h"


namespace eckit {
namespace linalg {
namespace sparse {

namespace {

struct MKLSparseHandle {
    sparse_matrix_t value = nullptr;

    ~MKLSparseHandle() {
        if (value != nullptr) {
            mkl_sparse_destroy(value);
        }
    }

    operator sparse_matrix_t() const { return value; }
};

void checkSparseStatus(sparse_status_t status, const char* call) {
    if (status != SPARSE_STATUS_SUCCESS) {
        throw SeriousBug(std::string(call) + " failed with MKL sparse status " + std::to_string(status), Here());
    }
}

matrix_descr generalMatrixDescription() {
    matrix_descr description{};
    description.type = SPARSE_MATRIX_TYPE_GENERAL;
    description.mode = SPARSE_FILL_MODE_FULL;
    description.diag = SPARSE_DIAG_NON_UNIT;
    return description;
}

MKLSparseHandle createSparseMatrixHandle(const SparseMatrix& A) {
    MKLSparseHandle handle;
    auto* outer = const_cast<MKL_INT*>(static_cast<const MKL_INT*>(A.outer()));
    auto* inner = const_cast<MKL_INT*>(static_cast<const MKL_INT*>(A.inner()));
    auto* data  = const_cast<double*>(static_cast<const double*>(A.data()));

    checkSparseStatus(mkl_sparse_d_create_csr(&handle.value, SPARSE_INDEX_BASE_ZERO, static_cast<MKL_INT>(A.rows()),
                                              static_cast<MKL_INT>(A.cols()), outer, outer + 1, inner, data),
                      "mkl_sparse_d_create_csr");
    checkSparseStatus(mkl_sparse_optimize(handle), "mkl_sparse_optimize");
    return handle;
}

}  // namespace


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

    const auto description = generalMatrixDescription();
    const auto handle      = createSparseMatrixHandle(A);

    checkSparseStatus(
        mkl_sparse_d_mv(SPARSE_OPERATION_NON_TRANSPOSE, alpha, handle, description, x.data(), beta, y.data()),
        "mkl_sparse_d_mv");
}


void LinearAlgebraMKL::spmm(const SparseMatrix& A, const Matrix& B, Matrix& C) const {
    ASSERT(A.cols() == B.rows());
    ASSERT(A.rows() == C.rows());
    ASSERT(B.cols() == C.cols());

    // We expect indices to be 0-based
    ASSERT(A.outer()[0] == 0);

    const auto m = static_cast<MKL_INT>(A.rows());
    const auto n = static_cast<MKL_INT>(C.cols());
    const auto k = static_cast<MKL_INT>(A.cols());

    const auto description = generalMatrixDescription();
    const auto handle      = createSparseMatrixHandle(A);

    checkSparseStatus(mkl_sparse_d_mm(SPARSE_OPERATION_NON_TRANSPOSE, alpha, handle, description,
                                      SPARSE_LAYOUT_COLUMN_MAJOR, B.data(), n, k, beta, C.data(), m),
                      "mkl_sparse_d_mm");
}


void LinearAlgebraMKL::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    static const sparse::LinearAlgebraGeneric generic;
    generic.dsptd(x, A, y, B);
}


}  // namespace sparse
}  // namespace linalg
}  // namespace eckit
