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

    CALL_CUDA(cudaMalloc((void**)&d_A_rowptr, sizeArowptr));
    CALL_CUDA(cudaMalloc((void**)&d_A_colidx, sizeAcolidx));
    CALL_CUDA(cudaMalloc((void**)&d_A_values, sizeAvalues));
    CALL_CUDA(cudaMalloc((void**)&d_x, sizex));
    CALL_CUDA(cudaMalloc((void**)&d_y, sizey));

    CALL_CUDA(cudaMemcpy(d_A_rowptr, A.outer(), sizeArowptr, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(d_A_colidx, A.inner(), sizeAcolidx, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(d_A_values, A.data(), sizeAvalues, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(d_x, x.data(), sizex, cudaMemcpyHostToDevice));

    cusparseHandle_t handle;
    CALL_CUSPARSE(cusparseCreate(&handle));

    cusparseSpMatDescr_t matA;
    CALL_CUSPARSE( cusparseCreateCsr(
        &matA,
        A.rows(), A.cols(), A.nonZeros(),
        d_A_rowptr,
        d_A_colidx,
        d_A_values,
        CUSPARSE_INDEX_32I,
        CUSPARSE_INDEX_32I,
        CUSPARSE_INDEX_BASE_ZERO,
        CUDA_R_64F) );

    cusparseDnVecDescr_t vecX;
    CALL_CUSPARSE( cusparseCreateDnVec(
        &vecX,
        x.size(),
        d_x,
        CUDA_R_64F) );

    cusparseDnVecDescr_t vecY;
    CALL_CUSPARSE( cusparseCreateDnVec(
        &vecY,
        y.size(),
        d_y,
        CUDA_R_64F) );

    const Scalar alpha = 1.0;
    const Scalar beta  = 0.0;

    // Determine buffer size
    size_t bufferSize = 0;
    CALL_CUSPARSE( cusparseSpMV_bufferSize(
        handle,
        CUSPARSE_OPERATION_NON_TRANSPOSE,
        &alpha,
        matA,
        vecX,
        &beta,
        vecY,
        CUDA_R_64F,
        CUSPARSE_SPMV_ALG_DEFAULT,
        &bufferSize) );

    // Allocate buffer
    char* buffer;
    CALL_CUDA( cudaMalloc(&buffer, bufferSize) );

    // Perform SpMV
    // y = alpha * A * x + beta * y
    CALL_CUSPARSE( cusparseSpMV(
        handle,
        CUSPARSE_OPERATION_NON_TRANSPOSE,
        &alpha,
        matA,
        vecX,
        &beta,
        vecY,
        CUDA_R_64F,
        CUSPARSE_SPMV_ALG_DEFAULT,
        buffer) );

    // Copy result back to host
    CALL_CUDA(cudaMemcpy(y.data(), d_y, sizey, cudaMemcpyDeviceToHost));

    CALL_CUSPARSE( cusparseDestroyDnVec(vecY) );
    CALL_CUSPARSE( cusparseDestroyDnVec(vecX) );
    CALL_CUSPARSE( cusparseDestroySpMat(matA) );
    CALL_CUSPARSE( cusparseDestroy(handle) );


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

    CALL_CUDA(cudaMalloc((void**)&d_A_rowptr, sizeArowptr));
    CALL_CUDA(cudaMalloc((void**)&d_A_colidx, sizeAcolidx));
    CALL_CUDA(cudaMalloc((void**)&d_A_values, sizeAvalues));
    CALL_CUDA(cudaMalloc((void**)&d_B, sizeB));
    CALL_CUDA(cudaMalloc((void**)&d_C, sizeC));

    CALL_CUDA(cudaMemcpy(d_A_rowptr, A.outer(), sizeArowptr, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(d_A_colidx, A.inner(), sizeAcolidx, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(d_A_values, A.data(), sizeAvalues, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(d_B, B.data(), sizeB, cudaMemcpyHostToDevice));

    cusparseHandle_t handle;
    CALL_CUSPARSE(cusparseCreate(&handle));

    cusparseSpMatDescr_t matA;
    CALL_CUSPARSE( cusparseCreateCsr(
        &matA,
        A.rows(), A.cols(), A.nonZeros(),
        d_A_rowptr,
        d_A_colidx,
        d_A_values,
        CUSPARSE_INDEX_32I,
        CUSPARSE_INDEX_32I,
        CUSPARSE_INDEX_BASE_ZERO,
        CUDA_R_64F) );

    // Create dense matrix descriptors
    cusparseDnMatDescr_t matB;
    CALL_CUSPARSE(cusparseCreateDnMat(
        &matB,
        B.rows(), // rows
        B.cols(), // cols
        B.rows(), // leading dimension
        d_B,
        CUDA_R_64F,
        CUSPARSE_ORDER_COL) );

    cusparseDnMatDescr_t matC;
    CALL_CUSPARSE(cusparseCreateDnMat(
        &matC,
        C.rows(), // rows
        C.cols(), // cols
        C.rows(), // leading dimension
        d_C,
        CUDA_R_64F,
        CUSPARSE_ORDER_COL) );

    const Scalar alpha = 1.0;
    const Scalar beta  = 0.0;

    size_t bufferSize = 0;
    CALL_CUSPARSE(cusparseSpMM_bufferSize(
        handle,
        CUSPARSE_OPERATION_NON_TRANSPOSE,
        CUSPARSE_OPERATION_NON_TRANSPOSE,
        &alpha,
        matA,
        matB,
        &beta,
        matC,
        CUDA_R_64F,
        CUSPARSE_SPMM_ALG_DEFAULT,
        &bufferSize));

    // Allocate buffer
    char* buffer;
    CALL_CUDA(cudaMalloc(&buffer, bufferSize));

    // Perform SpMM
    CALL_CUSPARSE(cusparseSpMM(
        handle,
        CUSPARSE_OPERATION_NON_TRANSPOSE,
        CUSPARSE_OPERATION_NON_TRANSPOSE,
        &alpha,
        matA,
        matB,
        &beta,
        matC,
        CUDA_R_64F,
        CUSPARSE_SPMM_ALG_DEFAULT,
        buffer));

    CALL_CUDA(cudaMemcpy(C.data(), d_C, sizeC, cudaMemcpyDeviceToHost));

    CALL_CUSPARSE(cusparseDestroy(handle));
    CALL_CUSPARSE(cusparseDestroyDnMat(matC));
    CALL_CUSPARSE(cusparseDestroyDnMat(matB));
    CALL_CUSPARSE(cusparseDestroySpMat(matA));

    CALL_CUDA(cudaFree(buffer));
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
