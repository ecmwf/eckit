/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/dense/LinearAlgebraCUDA.h"

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/Vector.h"
#include "eckit/linalg/detail/CUDA.h"


namespace eckit {
namespace linalg {
namespace dense {


static const LinearAlgebraCUDA __la("cuda");


void LinearAlgebraCUDA::print(std::ostream& out) const {
    out << "LinearAlgebraCUDA[]";
}


Scalar LinearAlgebraCUDA::dot(const Vector& x, const Vector& y) const {
    ASSERT(x.size() == y.size());

    const auto size = Size(x.size() * sizeof(Scalar));
    Scalar r;

    Scalar* d_x;  ///< device memory vector x
    Scalar* d_y;  ///< device memory vector y
    cublasHandle_t handle;

    CALL_CUDA(cudaMalloc((void**)&d_x, size));
    CALL_CUDA(cudaMalloc((void**)&d_y, size));

    CALL_CUBLAS(cublasCreate(&handle));

    CALL_CUDA(cudaMemcpy(d_x, x.data(), size, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(d_y, y.data(), size, cudaMemcpyHostToDevice));

    // cublasStatus_t cublasDdot (cublasHandle_t handle, int n,
    //                            const double *x, int incx,
    //                            const double *y, int incy,
    //                            double *result)
    CALL_CUBLAS(cublasDdot(handle, x.size(), d_x, 1, d_y, 1, &r));

    CALL_CUBLAS(cublasDestroy(handle));

    CALL_CUDA(cudaFree(d_x));
    CALL_CUDA(cudaFree(d_y));

    return r;
}


void LinearAlgebraCUDA::gemv(const Matrix& A, const Vector& x, Vector& y) const {
    ASSERT(x.size() == A.cols());
    ASSERT(y.size() == A.rows());

    const auto sizeA = Size(A.rows() * A.cols() * sizeof(Scalar));
    const auto sizex = Size(A.cols() * sizeof(Scalar));
    const auto sizey = Size(A.rows() * sizeof(Scalar));

    Scalar* d_A;  ///< device memory matrix A
    Scalar* d_x;  ///< device memory vector x
    Scalar* d_y;  ///< device memory vector y
    cublasHandle_t handle;

    CALL_CUDA(cudaMalloc((void**)&d_A, sizeA));
    CALL_CUDA(cudaMalloc((void**)&d_x, sizex));
    CALL_CUDA(cudaMalloc((void**)&d_y, sizey));

    CALL_CUBLAS(cublasCreate(&handle));

    CALL_CUDA(cudaMemcpy(d_A, A.data(), sizeA, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(d_x, x.data(), sizex, cudaMemcpyHostToDevice));

    const Scalar alpha = 1.0;
    const Scalar beta  = 0.0;

    // cublasStatus_t cublasDgemv(cublasHandle_t handle, cublasOperation_t trans,
    //                            int m, int n,
    //                            const double *alpha, const double *A, int lda, const double *x, int incx,
    //                            const double *beta, double *y, int incy)
    CALL_CUBLAS(cublasDgemv(handle, CUBLAS_OP_N, A.rows(), A.cols(), &alpha, d_A, A.rows(), d_x, 1, &beta, d_y, 1));

    CALL_CUDA(cudaMemcpy(y.data(), d_y, sizey, cudaMemcpyDeviceToHost));

    CALL_CUBLAS(cublasDestroy(handle));

    CALL_CUDA(cudaFree(d_A));
    CALL_CUDA(cudaFree(d_x));
    CALL_CUDA(cudaFree(d_y));
}


void LinearAlgebraCUDA::gemm(const Matrix& A, const Matrix& B, Matrix& C) const {
    ASSERT(A.cols() == B.rows());
    ASSERT(A.rows() == C.rows());
    ASSERT(B.cols() == C.cols());

    const auto sizeA = Size(A.rows() * A.cols() * sizeof(Scalar));
    const auto sizeB = Size(B.rows() * B.cols() * sizeof(Scalar));
    const auto sizeC = Size(A.rows() * B.cols() * sizeof(Scalar));

    Scalar* d_A;  ///< device memory matrix A
    Scalar* d_B;  ///< device memory matrix B
    Scalar* d_C;  ///< device memory matrix C
    cublasHandle_t handle;

    CALL_CUDA(cudaMalloc((void**)&d_A, sizeA));
    CALL_CUDA(cudaMalloc((void**)&d_B, sizeB));
    CALL_CUDA(cudaMalloc((void**)&d_C, sizeC));

    CALL_CUBLAS(cublasCreate(&handle));

    CALL_CUDA(cudaMemcpy(d_A, A.data(), sizeA, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(d_B, B.data(), sizeB, cudaMemcpyHostToDevice));

    const Scalar alpha = 1.0;
    const Scalar beta  = 0.0;

    // cublasStatus_t cublasDgemm(cublasHandle_t handle, cublasOperation_t transa, cublasOperation_t transb,
    //                            int m, int n, int k,
    //                            const double *alpha, const double *A, int lda, const double *B, int ldb,
    //                            const double *beta, double *C, int ldc)
    CALL_CUBLAS(cublasDgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, A.rows(), B.cols(), A.cols(), &alpha, d_A, A.rows(), d_B,
                            B.rows(), &beta, d_C, A.rows()));

    CALL_CUDA(cudaMemcpy(C.data(), d_C, sizeC, cudaMemcpyDeviceToHost));

    CALL_CUBLAS(cublasDestroy(handle));

    CALL_CUDA(cudaFree(d_A));
    CALL_CUDA(cudaFree(d_B));
    CALL_CUDA(cudaFree(d_C));
}


}  // namespace dense
}  // namespace linalg
}  // namespace eckit
