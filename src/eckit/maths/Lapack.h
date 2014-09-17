#ifndef eckit_maths_lapack_h
#define eckit_maths_lapack_h

#include "eckit/eckit_config.h"

namespace eckit {

namespace maths {

namespace lapack {


template < typename scalar >
void getrf(int* M, int* N, scalar* data, int* lda, int* ipiv, int* info);

template < typename scalar >
void getri(int* M, scalar* data, int* lda, int* ipiv, scalar* work, int* lwork, int* info);


//////////////////////////////////////////////////////////////////////////////////////
// Template specialisations
//////////////////////////////////////////////////////////////////////////////////////

#ifdef HAVE_LAPACK

extern "C" {
    void dgetrf_ (int *M, int *N, double *A, int *lda, int *ipiv, int *info);
    void sgetrf_ (int *M, int *N, float  *A, int *lda, int *ipiv, int *info);
    void dgetri_ (int *M, double *A, int *lda, int *ipiv, double *work, int *lwork, int *info);
    void sgetri_ (int *M, float  *A, int *lda, int *ipiv, float  *work, int *lwork, int *info);
}

template <>
void getrf<double>(int* M, int* N, double* data, int* lda, int* ipiv, int* info)
{
	dgetrf_(M, N, data, lda, ipiv, info);
}
template <>
void getrf<float>(int* M, int* N, float* data, int* lda, int* ipiv, int* info)
{
	sgetrf_(M, N, data, lda, ipiv, info);
}

template <>
void getri<double>(int* M, double* data, int* lda, int* ipiv, double* work, int* lwork, int* info)
{
	dgetri_(M, data, lda, ipiv, work, lwork, info);
}

template <>
void getri<float>(int* M, float* data, int* lda, int* ipiv, float* work, int* lwork, int* info)
{
	sgetri_(M, data, lda, ipiv, work, lwork, info);
}

#endif

} // namespace lapack

} // namespace maths

} // namespace eckit

#endif
