/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/sparse/LinearAlgebraCUDA.h"

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"
#include "eckit/linalg/detail/CUDA.h"
#include "eckit/linalg/sparse/LinearAlgebraGeneric.h"


namespace eckit {
namespace linalg {
namespace sparse {


static const LinearAlgebraCUDA __la("cuda");


void LinearAlgebraCUDA::print(std::ostream& out) const {
    out << "LinearAlgebraCUDA[]";
}


void LinearAlgebraCUDA::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    ASSERT(x.size() == A.cols() && y.size() == A.rows());
    // We expect indices to be 0-based
    ASSERT(A.outer()[0] == 0);
    const Size sizeArowptr = (A.rows() + 1) * sizeof(Index);
    const Size sizeAcolidx = A.nonZeros() * sizeof(Index);
    const Size sizeAvalues = A.nonZeros() * sizeof(Scalar);
    const Size sizex       = A.cols() * sizeof(Scalar);
    const Size sizey       = A.rows() * sizeof(Scalar);

    Index* d_A_rowptr;   ///< device memory matrix A row pointers
    Index* d_A_colidx;   ///< device memory matrix A col indices
    Scalar* d_A_values;  ///< device memory matrix A values
    Scalar* d_x;         ///< device memory vector x
    Scalar* d_y;         ///< device memory vector y
    cusparseHandle_t handle;
    cusparseMatDescr_t descr;

    CALL_CUDA(cudaMalloc((void**)&d_A_rowptr, sizeArowptr));
    CALL_CUDA(cudaMalloc((void**)&d_A_colidx, sizeAcolidx));
    CALL_CUDA(cudaMalloc((void**)&d_A_values, sizeAvalues));
    CALL_CUDA(cudaMalloc((void**)&d_x, sizex));
    CALL_CUDA(cudaMalloc((void**)&d_y, sizey));

    CALL_CUSPARSE(cusparseCreate(&handle));
    CALL_CUSPARSE(cusparseCreateMatDescr(&descr));
    cusparseSetMatType(descr, CUSPARSE_MATRIX_TYPE_GENERAL);
    cusparseSetMatIndexBase(descr, CUSPARSE_INDEX_BASE_ZERO);

    CALL_CUDA(cudaMemcpy(d_A_rowptr, A.outer(), sizeArowptr, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(d_A_colidx, A.inner(), sizeAcolidx, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(d_A_values, A.data(), sizeAvalues, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(d_x, x.data(), sizex, cudaMemcpyHostToDevice));

    const Scalar alpha = 1.0;
    const Scalar beta  = 0.0;
    // cusparseStatus_t
    // cusparseDcsrmv(cusparseHandle_t handle, cusparseOperation_t transA,
    //                int m, int n, int nnz,
    //                const double *alpha, const cusparseMatDescr_t descrA,
    //                const double *csrValA, const int *csrRowPtrA, const int *csrColIndA,
    //                const double *x, const double *beta, double *y)
    CALL_CUSPARSE(cusparseDcsrmv(handle, CUSPARSE_OPERATION_NON_TRANSPOSE, A.rows(), A.cols(), A.nonZeros(), &alpha,
                                 descr, d_A_values, d_A_rowptr, d_A_colidx, d_x, &beta, d_y));

    CALL_CUDA(cudaMemcpy(y.data(), d_y, sizey, cudaMemcpyDeviceToHost));

    CALL_CUSPARSE(cusparseDestroyMatDescr(descr));
    CALL_CUSPARSE(cusparseDestroy(handle));

    CALL_CUDA(cudaFree(d_A_rowptr));
    CALL_CUDA(cudaFree(d_A_colidx));
    CALL_CUDA(cudaFree(d_A_values));
    CALL_CUDA(cudaFree(d_x));
    CALL_CUDA(cudaFree(d_y));
}


void LinearAlgebraCUDA::spmm(const SparseMatrix& A, const Matrix& B, Matrix& C) const {
    ASSERT(A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols());
    // We expect indices to be 0-based
    ASSERT(A.outer()[0] == 0);
    const Size sizeArowptr = (A.rows() + 1) * sizeof(Index);
    const Size sizeAcolidx = A.nonZeros() * sizeof(Index);
    const Size sizeAvalues = A.nonZeros() * sizeof(Scalar);
    const Size sizeB       = B.rows() * B.cols() * sizeof(Scalar);
    const Size sizeC       = A.rows() * B.cols() * sizeof(Scalar);

    Index* d_A_rowptr;   ///< device memory matrix A row pointers
    Index* d_A_colidx;   ///< device memory matrix A col indices
    Scalar* d_A_values;  ///< device memory matrix A values
    Scalar* d_B;         ///< device memory matrix B
    Scalar* d_C;         ///< device memory matrix C
    cusparseHandle_t handle;
    cusparseMatDescr_t descr;

    CALL_CUDA(cudaMalloc((void**)&d_A_rowptr, sizeArowptr));
    CALL_CUDA(cudaMalloc((void**)&d_A_colidx, sizeAcolidx));
    CALL_CUDA(cudaMalloc((void**)&d_A_values, sizeAvalues));
    CALL_CUDA(cudaMalloc((void**)&d_B, sizeB));
    CALL_CUDA(cudaMalloc((void**)&d_C, sizeC));

    CALL_CUSPARSE(cusparseCreate(&handle));
    CALL_CUSPARSE(cusparseCreateMatDescr(&descr));
    cusparseSetMatType(descr, CUSPARSE_MATRIX_TYPE_GENERAL);
    cusparseSetMatIndexBase(descr, CUSPARSE_INDEX_BASE_ZERO);

    CALL_CUDA(cudaMemcpy(d_A_rowptr, A.outer(), sizeArowptr, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(d_A_colidx, A.inner(), sizeAcolidx, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(d_A_values, A.data(), sizeAvalues, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(d_B, B.data(), sizeB, cudaMemcpyHostToDevice));

    // FIXME: Should we transpose B and use cusparseDcsrmm2 instread?
    // http://docs.nvidia.com/cuda/cusparse/index.html#cusparse-lt-t-gt-csrmm2
    const Scalar alpha = 1.0;
    const Scalar beta  = 0.0;
    // cusparseStatus_t
    // cusparseDcsrmm(cusparseHandle_t handle, cusparseOperation_t transA,
    //                int m, int n, int k, int nnz,
    //                const double *alpha, const cusparseMatDescr_t descrA,
    //                const double *csrValA, const int *csrRowPtrA, const int *csrColIndA,
    //                const double *B, int ldb, const double *beta, double *C, int ldc)
    CALL_CUSPARSE(cusparseDcsrmm(handle, CUSPARSE_OPERATION_NON_TRANSPOSE, A.rows(), A.cols(), B.cols(), A.nonZeros(),
                                 &alpha, descr, d_A_values, d_A_rowptr, d_A_colidx, d_B, B.rows(), &beta, d_C,
                                 C.rows()));

    CALL_CUDA(cudaMemcpy(C.data(), d_C, sizeC, cudaMemcpyDeviceToHost));

    CALL_CUSPARSE(cusparseDestroyMatDescr(descr));
    CALL_CUSPARSE(cusparseDestroy(handle));

    CALL_CUDA(cudaFree(d_A_rowptr));
    CALL_CUDA(cudaFree(d_A_colidx));
    CALL_CUDA(cudaFree(d_A_values));
    CALL_CUDA(cudaFree(d_B));
    CALL_CUDA(cudaFree(d_C));
}


void LinearAlgebraCUDA::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    static const sparse::LinearAlgebraGeneric generic;
    generic.dsptd(x, A, y, B);
}


}  // namespace sparse
}  // namespace linalg
}  // namespace eckit
