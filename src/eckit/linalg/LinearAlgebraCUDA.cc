/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/eckit_config.h"

#ifdef ECKIT_HAVE_CUDA

#include "eckit/linalg/LinearAlgebraCUDA.h"

#include <cuda_runtime.h>
#include <cublas_v2.h>
#include <cusparse.h>

//-----------------------------------------------------------------------------

#define CALL_CUDA(e) \
{ cudaError_t error; \
    if( ( error = e ) != cudaSuccess ) printf("%s failed with error code %d @ %s +%d\n", #e, error, __FILE__, __LINE__), exit(EXIT_FAILURE); \
}

#define CALL_CUBLAS(e) \
{ cublasStatus_t error; \
    if( ( error = e ) != CUBLAS_STATUS_SUCCESS ) printf("%s failed with error code %d @ %s +%d\n", #e, error, __FILE__, __LINE__), exit(EXIT_FAILURE); \
}

#define CALL_CUSPARSE(e) \
{ cusparseStatus_t error; \
    if( ( error = e ) != CUSPARSE_STATUS_SUCCESS ) printf("%s failed with error code %d @ %s +%d\n", #e, error, __FILE__, __LINE__), exit(EXIT_FAILURE); \
}

//-----------------------------------------------------------------------------

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"

//-----------------------------------------------------------------------------

namespace eckit {
namespace linalg {

//-----------------------------------------------------------------------------

LinearAlgebraCUDA::LinearAlgebraCUDA() : LinearAlgebra("cuda") {}

//-----------------------------------------------------------------------------

Scalar LinearAlgebraCUDA::dot(const Vector& x, const Vector& y) const {
    ASSERT( x.size() == y.size() );
    const size_t size = x.size()*sizeof(Scalar);
    Scalar r;

    Scalar* d_x; ///< device memory vector x
    Scalar* d_y; ///< device memory vector y
    cublasHandle_t handle;

    CALL_CUDA( cudaMalloc((void**) &d_x, size) );
    CALL_CUDA( cudaMalloc((void**) &d_y, size) );

    CALL_CUBLAS( cublasCreate(&handle) );

    CALL_CUDA( cudaMemcpy(d_x, x.data(), size, cudaMemcpyHostToDevice) );
    CALL_CUDA( cudaMemcpy(d_y, y.data(), size, cudaMemcpyHostToDevice) );

    // cublasStatus_t cublasDdot (cublasHandle_t handle, int n,
    //                            const double *x, int incx,
    //                            const double *y, int incy,
    //                            double *result)
    CALL_CUBLAS( cublasDdot(handle, x.size(), d_x, 1, d_y, 1, &r) );

    CALL_CUBLAS( cublasDestroy(handle) );

    CALL_CUDA( cudaFree(d_x) );
    CALL_CUDA( cudaFree(d_y) );

    return r;
}

//-----------------------------------------------------------------------------

void LinearAlgebraCUDA::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT( x.size() == A.cols() && y.size() == A.rows() );
    const size_t sizeA = A.rows()*A.cols()*sizeof(Scalar);
    const size_t sizex = A.cols()*sizeof(Scalar);
    const size_t sizey = A.rows()*sizeof(Scalar);

    Scalar* d_A; ///< device memory matrix A
    Scalar* d_x; ///< device memory vector x
    Scalar* d_y; ///< device memory vector y
    cublasHandle_t handle;

    CALL_CUDA( cudaMalloc((void**) &d_A, sizeA) );
    CALL_CUDA( cudaMalloc((void**) &d_x, sizex) );
    CALL_CUDA( cudaMalloc((void**) &d_y, sizey) );

    CALL_CUBLAS( cublasCreate(&handle) );

    CALL_CUDA( cudaMemcpy(d_A, A.data(), sizeA, cudaMemcpyHostToDevice) );
    CALL_CUDA( cudaMemcpy(d_x, x.data(), sizex, cudaMemcpyHostToDevice) );

    const Scalar alpha = 1.0;
    const Scalar beta  = 0.0;
    // cublasStatus_t cublasDgemv(cublasHandle_t handle, cublasOperation_t trans,
    //                            int m, int n,
    //                            const double *alpha, const double *A, int lda, const double *x, int incx,
    //                            const double *beta, double *y, int incy)
    CALL_CUBLAS( cublasDgemv(handle, CUBLAS_OP_N,
                             A.rows(), A.cols(),
                             &alpha, d_A, A.rows(), d_x, 1,
                             &beta, d_y, 1) );

    CALL_CUDA( cudaMemcpy(y.data(), d_y, sizey, cudaMemcpyDeviceToHost) );

    CALL_CUBLAS( cublasDestroy(handle) );

    CALL_CUDA( cudaFree(d_A) );
    CALL_CUDA( cudaFree(d_x) );
    CALL_CUDA( cudaFree(d_y) );
}

//-----------------------------------------------------------------------------

void LinearAlgebraCUDA::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    ASSERT( A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols() );
    const size_t sizeA = A.rows()*A.cols()*sizeof(Scalar);
    const size_t sizeB = B.rows()*B.cols()*sizeof(Scalar);
    const size_t sizeC = A.rows()*B.cols()*sizeof(Scalar);

    Scalar* d_A; ///< device memory matrix A
    Scalar* d_B; ///< device memory matrix B
    Scalar* d_C; ///< device memory matrix C
    cublasHandle_t handle;

    CALL_CUDA( cudaMalloc((void**) &d_A, sizeA) );
    CALL_CUDA( cudaMalloc((void**) &d_B, sizeB) );
    CALL_CUDA( cudaMalloc((void**) &d_C, sizeC) );

    CALL_CUBLAS( cublasCreate(&handle) );

    CALL_CUDA( cudaMemcpy(d_A, A.data(), sizeA, cudaMemcpyHostToDevice) );
    CALL_CUDA( cudaMemcpy(d_B, B.data(), sizeB, cudaMemcpyHostToDevice) );

    const Scalar alpha = 1.0;
    const Scalar beta  = 0.0;
    // cublasStatus_t cublasDgemm(cublasHandle_t handle, cublasOperation_t transa, cublasOperation_t transb,
    //                            int m, int n, int k,
    //                            const double *alpha, const double *A, int lda, const double *B, int ldb,
    //                            const double *beta, double *C, int ldc)
    CALL_CUBLAS( cublasDgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N,
                             A.rows(), B.cols(), A.cols(),
                             &alpha, d_A, A.rows(), d_B, B.rows(),
                             &beta, d_C, A.rows()) );

    CALL_CUDA( cudaMemcpy(C.data(), d_C, sizeC, cudaMemcpyDeviceToHost) );

    CALL_CUBLAS( cublasDestroy(handle) );

    CALL_CUDA( cudaFree(d_A) );
    CALL_CUDA( cudaFree(d_B) );
    CALL_CUDA( cudaFree(d_C) );
}

//-----------------------------------------------------------------------------

void LinearAlgebraCUDA::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
    ASSERT( x.size() == A.cols() && y.size() == A.rows() );
    // We expect indices to be 0-based
    ASSERT( A.outer()[0] == 0 );
    const size_t sizeAnnz = A.nonZeros()*sizeof(Scalar);
    const size_t sizeAptr = (A.rows()+1)*sizeof(Scalar);
    const size_t sizex = A.cols()*sizeof(Scalar);
    const size_t sizey = A.rows()*sizeof(Scalar);

    Index* d_A_rowptr; ///< device memory matrix A row pointers
    Index* d_A_colidx; ///< device memory matrix A col indices
    Scalar* d_A_values; ///< device memory matrix A values
    Scalar* d_x; ///< device memory vector x
    Scalar* d_y; ///< device memory vector y
    cusparseHandle_t handle;
    cusparseMatDescr_t descr;

    CALL_CUDA( cudaMalloc((void**) &d_A_rowptr, sizeAptr) );
    CALL_CUDA( cudaMalloc((void**) &d_A_colidx, sizeAnnz) );
    CALL_CUDA( cudaMalloc((void**) &d_A_values, sizeAnnz) );
    CALL_CUDA( cudaMalloc((void**) &d_x, sizex) );
    CALL_CUDA( cudaMalloc((void**) &d_y, sizey) );

    CALL_CUSPARSE( cusparseCreate(&handle) );
    CALL_CUSPARSE( cusparseCreateMatDescr(&descr) );
    cusparseSetMatType(descr,CUSPARSE_MATRIX_TYPE_GENERAL);
    cusparseSetMatIndexBase(descr,CUSPARSE_INDEX_BASE_ZERO);

    CALL_CUDA( cudaMemcpy(d_A_rowptr, A.outer(), sizeAptr, cudaMemcpyHostToDevice) );
    CALL_CUDA( cudaMemcpy(d_A_colidx, A.inner(), sizeAnnz, cudaMemcpyHostToDevice) );
    CALL_CUDA( cudaMemcpy(d_A_values, A.data(),  sizeAnnz, cudaMemcpyHostToDevice) );
    CALL_CUDA( cudaMemcpy(d_x,        x.data(),  sizex,    cudaMemcpyHostToDevice) );

    const Scalar alpha = 1.0;
    const Scalar beta  = 0.0;
    // cusparseStatus_t
    // cusparseDcsrmv(cusparseHandle_t handle, cusparseOperation_t transA,
    //                int m, int n, int nnz,
    //                const double *alpha, const cusparseMatDescr_t descrA,
    //                const double *csrValA, const int *csrRowPtrA, const int *csrColIndA,
    //                const double *x, const double *beta, double *y)
    CALL_CUSPARSE( cusparseDcsrmv(handle, CUSPARSE_OPERATION_NON_TRANSPOSE,
                                  A.rows(), A.cols(), A.nonZeros(),
                                  &alpha, descr,
                                  d_A_values, d_A_rowptr, d_A_colidx,
                                  d_x, &beta, d_y) );

    CALL_CUDA( cudaMemcpy(y.data(), d_y, sizey, cudaMemcpyDeviceToHost) );

    CALL_CUSPARSE( cusparseDestroyMatDescr(descr) );
    CALL_CUSPARSE( cusparseDestroy(handle) );

    CALL_CUDA( cudaFree(d_A_rowptr) );
    CALL_CUDA( cudaFree(d_A_colidx) );
    CALL_CUDA( cudaFree(d_A_values) );
    CALL_CUDA( cudaFree(d_x) );
    CALL_CUDA( cudaFree(d_y) );
}

//-----------------------------------------------------------------------------

void LinearAlgebraCUDA::spmm(const SparseMatrix& A, const Matrix& B, Matrix& C) const {
    ASSERT( A.cols() == B.rows() && A.rows() == C.rows() && B.cols() == C.cols() );
    // We expect indices to be 0-based
    ASSERT( A.outer()[0] == 0 );
    const size_t sizeAnnz = A.nonZeros()*sizeof(Scalar);
    const size_t sizeAptr = (A.rows()+1)*sizeof(Scalar);
    const size_t sizeB = B.rows()*B.cols()*sizeof(Scalar);
    const size_t sizeC = A.rows()*B.cols()*sizeof(Scalar);

    Index* d_A_rowptr; ///< device memory matrix A row pointers
    Index* d_A_colidx; ///< device memory matrix A col indices
    Scalar* d_A_values; ///< device memory matrix A values
    Scalar* d_B; ///< device memory matrix B
    Scalar* d_C; ///< device memory matrix C
    cusparseHandle_t handle;
    cusparseMatDescr_t descr;

    CALL_CUDA( cudaMalloc((void**) &d_A_rowptr, sizeAptr) );
    CALL_CUDA( cudaMalloc((void**) &d_A_colidx, sizeAnnz) );
    CALL_CUDA( cudaMalloc((void**) &d_A_values, sizeAnnz) );
    CALL_CUDA( cudaMalloc((void**) &d_B, sizeB) );
    CALL_CUDA( cudaMalloc((void**) &d_C, sizeC) );

    CALL_CUSPARSE( cusparseCreate(&handle) );
    CALL_CUSPARSE( cusparseCreateMatDescr(&descr) );
    cusparseSetMatType(descr,CUSPARSE_MATRIX_TYPE_GENERAL);
    cusparseSetMatIndexBase(descr,CUSPARSE_INDEX_BASE_ZERO);

    CALL_CUDA( cudaMemcpy(d_A_rowptr, A.outer(), sizeAptr, cudaMemcpyHostToDevice) );
    CALL_CUDA( cudaMemcpy(d_A_colidx, A.inner(), sizeAnnz, cudaMemcpyHostToDevice) );
    CALL_CUDA( cudaMemcpy(d_A_values, A.data(),  sizeAnnz, cudaMemcpyHostToDevice) );
    CALL_CUDA( cudaMemcpy(d_B,        B.data(),  sizeB,    cudaMemcpyHostToDevice) );

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
    CALL_CUSPARSE( cusparseDcsrmm(handle, CUSPARSE_OPERATION_NON_TRANSPOSE,
                                  A.rows(), A.cols(), B.cols(), A.nonZeros(),
                                  &alpha, descr,
                                  d_A_values, d_A_rowptr, d_A_colidx,
                                  d_B, B.rows(), &beta, d_C, C.rows()) );

    CALL_CUDA( cudaMemcpy(C.data(), d_C, sizeC, cudaMemcpyDeviceToHost) );

    CALL_CUSPARSE( cusparseDestroyMatDescr(descr) );
    CALL_CUSPARSE( cusparseDestroy(handle) );

    CALL_CUDA( cudaFree(d_A_rowptr) );
    CALL_CUDA( cudaFree(d_A_colidx) );
    CALL_CUDA( cudaFree(d_A_values) );
    CALL_CUDA( cudaFree(d_B) );
    CALL_CUDA( cudaFree(d_C) );
}

//-----------------------------------------------------------------------------

void LinearAlgebraCUDA::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    LinearAlgebra::getBackend("generic").dsptd(x, A, y, B);
}

//-----------------------------------------------------------------------------

void LinearAlgebraCUDA::print(std::ostream& out) const {
    out << "LinearAlgebraCUDA[]";
}

//-----------------------------------------------------------------------------

static LinearAlgebraCUDA linearAlgebraCUDA;

//-----------------------------------------------------------------------------

}  // namespace linalg
} // namespace eckit

#endif  // ECKIT_HAVE_CUDA
