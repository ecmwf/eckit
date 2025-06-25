/*
 * (C) Copyright 2025- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */


#include "eckit/linalg/sparse/LinearAlgebraHIP.h"

#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/linalg/Matrix.h"
#include "eckit/linalg/SparseMatrix.h"
#include "eckit/linalg/Vector.h"
#include "eckit/linalg/detail/HIP.h"
#include "eckit/linalg/sparse/LinearAlgebraGeneric.h"


namespace eckit {
namespace linalg {
namespace sparse {


static const LinearAlgebraHIP __la("hip");


void LinearAlgebraHIP::print(std::ostream& out) const {
    out << "LinearAlgebraHIP[]";
}


void LinearAlgebraHIP::spmv(const SparseMatrix& A, const Vector& x, Vector& y) const {
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

    CALL_HIP(hipMalloc((void**)&d_A_rowptr, sizeArowptr));
    CALL_HIP(hipMalloc((void**)&d_A_colidx, sizeAcolidx));
    CALL_HIP(hipMalloc((void**)&d_A_values, sizeAvalues));
    CALL_HIP(hipMalloc((void**)&d_x, sizex));
    CALL_HIP(hipMalloc((void**)&d_y, sizey));

    CALL_HIP(hipMemcpy(d_A_rowptr, A.outer(), sizeArowptr, hipMemcpyHostToDevice));
    CALL_HIP(hipMemcpy(d_A_colidx, A.inner(), sizeAcolidx, hipMemcpyHostToDevice));
    CALL_HIP(hipMemcpy(d_A_values, A.data(), sizeAvalues, hipMemcpyHostToDevice));
    CALL_HIP(hipMemcpy(d_x, x.data(), sizex, hipMemcpyHostToDevice));

    hipsparseHandle_t handle;
    CALL_HIPSPARSE(hipsparseCreate(&handle));

    hipsparseSpMatDescr_t matA;
    CALL_HIPSPARSE(hipsparseCreateCsr(&matA, A.rows(), A.cols(), A.nonZeros(), d_A_rowptr, d_A_colidx, d_A_values,
                                      HIPSPARSE_INDEX_32I, HIPSPARSE_INDEX_32I, HIPSPARSE_INDEX_BASE_ZERO, HIP_R_64F));

    hipsparseDnVecDescr_t vecX;
    CALL_HIPSPARSE(hipsparseCreateDnVec(&vecX, x.size(), d_x, HIP_R_64F));

    hipsparseDnVecDescr_t vecY;
    CALL_HIPSPARSE(hipsparseCreateDnVec(&vecY, y.size(), d_y, HIP_R_64F));

    const Scalar alpha = 1.0;
    const Scalar beta  = 0.0;

    // Determine buffer size
    size_t bufferSize = 0;
    CALL_HIPSPARSE(hipsparseSpMV_bufferSize(handle, HIPSPARSE_OPERATION_NON_TRANSPOSE, &alpha, matA, vecX, &beta, vecY,
                                            HIP_R_64F, HIPSPARSE_SPMV_ALG_DEFAULT, &bufferSize));

    // Allocate buffer
    char* buffer;
    CALL_HIP(hipMalloc(&buffer, bufferSize));

    // Perform SpMV
    // y = alpha * A * x + beta * y
    CALL_HIPSPARSE(hipsparseSpMV(handle, HIPSPARSE_OPERATION_NON_TRANSPOSE, &alpha, matA, vecX, &beta, vecY, HIP_R_64F,
                                 HIPSPARSE_SPMV_ALG_DEFAULT, buffer));

    // Copy result back to host
    CALL_HIP(hipMemcpy(y.data(), d_y, sizey, hipMemcpyDeviceToHost));

    CALL_HIPSPARSE(hipsparseDestroyDnVec(vecY));
    CALL_HIPSPARSE(hipsparseDestroyDnVec(vecX));
    CALL_HIPSPARSE(hipsparseDestroySpMat(matA));
    CALL_HIPSPARSE(hipsparseDestroy(handle));


    CALL_HIP(hipFree(d_A_rowptr));
    CALL_HIP(hipFree(d_A_colidx));
    CALL_HIP(hipFree(d_A_values));
    CALL_HIP(hipFree(d_x));
    CALL_HIP(hipFree(d_y));
}


void LinearAlgebraHIP::spmm(const SparseMatrix& A, const Matrix& B, Matrix& C) const {
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

    CALL_HIP(hipMalloc((void**)&d_A_rowptr, sizeArowptr));
    CALL_HIP(hipMalloc((void**)&d_A_colidx, sizeAcolidx));
    CALL_HIP(hipMalloc((void**)&d_A_values, sizeAvalues));
    CALL_HIP(hipMalloc((void**)&d_B, sizeB));
    CALL_HIP(hipMalloc((void**)&d_C, sizeC));

    CALL_HIP(hipMemcpy(d_A_rowptr, A.outer(), sizeArowptr, hipMemcpyHostToDevice));
    CALL_HIP(hipMemcpy(d_A_colidx, A.inner(), sizeAcolidx, hipMemcpyHostToDevice));
    CALL_HIP(hipMemcpy(d_A_values, A.data(), sizeAvalues, hipMemcpyHostToDevice));
    CALL_HIP(hipMemcpy(d_B, B.data(), sizeB, hipMemcpyHostToDevice));

    hipsparseHandle_t handle;
    CALL_HIPSPARSE(hipsparseCreate(&handle));

    hipsparseSpMatDescr_t matA;
    CALL_HIPSPARSE(hipsparseCreateCsr(&matA, A.rows(), A.cols(), A.nonZeros(), d_A_rowptr, d_A_colidx, d_A_values,
                                      HIPSPARSE_INDEX_32I, HIPSPARSE_INDEX_32I, HIPSPARSE_INDEX_BASE_ZERO, HIP_R_64F));

    // Create dense matrix descriptors
    hipsparseDnMatDescr_t matB;
    CALL_HIPSPARSE(hipsparseCreateDnMat(&matB,
                                        B.rows(),  // rows
                                        B.cols(),  // cols
                                        B.rows(),  // leading dimension
                                        d_B, HIP_R_64F, HIPSPARSE_ORDER_COL));

    hipsparseDnMatDescr_t matC;
    CALL_HIPSPARSE(hipsparseCreateDnMat(&matC,
                                        C.rows(),  // rows
                                        C.cols(),  // cols
                                        C.rows(),  // leading dimension
                                        d_C, HIP_R_64F, HIPSPARSE_ORDER_COL));

    const Scalar alpha = 1.0;
    const Scalar beta  = 0.0;

    size_t bufferSize = 0;
    CALL_HIPSPARSE(hipsparseSpMM_bufferSize(handle, HIPSPARSE_OPERATION_NON_TRANSPOSE,
                                            HIPSPARSE_OPERATION_NON_TRANSPOSE, &alpha, matA, matB, &beta, matC,
                                            HIP_R_64F, HIPSPARSE_SPMM_ALG_DEFAULT, &bufferSize));

    // Allocate buffer
    char* buffer;
    CALL_HIP(hipMalloc(&buffer, bufferSize));

    // Perform SpMM
    CALL_HIPSPARSE(hipsparseSpMM(handle, HIPSPARSE_OPERATION_NON_TRANSPOSE, HIPSPARSE_OPERATION_NON_TRANSPOSE, &alpha,
                                 matA, matB, &beta, matC, HIP_R_64F, HIPSPARSE_SPMM_ALG_DEFAULT, buffer));

    CALL_HIP(hipMemcpy(C.data(), d_C, sizeC, hipMemcpyDeviceToHost));

    CALL_HIPSPARSE(hipsparseDestroy(handle));
    CALL_HIPSPARSE(hipsparseDestroyDnMat(matC));
    CALL_HIPSPARSE(hipsparseDestroyDnMat(matB));
    CALL_HIPSPARSE(hipsparseDestroySpMat(matA));

    CALL_HIP(hipFree(buffer));
    CALL_HIP(hipFree(d_A_rowptr));
    CALL_HIP(hipFree(d_A_colidx));
    CALL_HIP(hipFree(d_A_values));
    CALL_HIP(hipFree(d_B));
    CALL_HIP(hipFree(d_C));
}


void LinearAlgebraHIP::dsptd(const Vector& x, const SparseMatrix& A, const Vector& y, SparseMatrix& B) const {
    static const sparse::LinearAlgebraGeneric generic;
    generic.dsptd(x, A, y, B);
}


}  // namespace sparse
}  // namespace linalg
}  // namespace eckit
