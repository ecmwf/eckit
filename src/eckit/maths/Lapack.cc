#ifndef eckit_maths_lapack_h
#define eckit_maths_lapack_h

#include "eckit/log/Log.h"
#include "eckit/maths/Lapack.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {
namespace maths {
namespace lapack {

//////////////////////////////////////////////////////////////////////////////////////
// Template specialisations
//////////////////////////////////////////////////////////////////////////////////////

#ifdef ECKIT_HAVE_LAPACK

extern "C" {
    void dgetrf_ (int *M, int *N, double *A, int *lda, int *ipiv, int *info);
    void sgetrf_ (int *M, int *N, float  *A, int *lda, int *ipiv, int *info);
    void dgetri_ (int *M, double *A, int *lda, int *ipiv, double *work, int *lwork, int *info);
    void sgetri_ (int *M, float  *A, int *lda, int *ipiv, float  *work, int *lwork, int *info);
}

void getrf(int* M, int* N, double* data, int* lda, int* ipiv, int* info)
{
	dgetrf_(M, N, data, lda, ipiv, info);
}

void getrf(int* M, int* N, float* data, int* lda, int* ipiv, int* info)
{
	sgetrf_(M, N, data, lda, ipiv, info);
}

void getri(int* M, double* data, int* lda, int* ipiv, double* work, int* lwork, int* info)
{
	dgetri_(M, data, lda, ipiv, work, lwork, info);
}

void getri(int* M, float* data, int* lda, int* ipiv, float* work, int* lwork, int* info)
{
	sgetri_(M, data, lda, ipiv, work, lwork, info);
}

#else

void getrf(int* M, int* N, double* data, int* lda, int* ipiv, int* info)
{
	NOTIMP;
}

void getrf(int* M, int* N, float* data, int* lda, int* ipiv, int* info)
{
	NOTIMP;
}

void getri(int* M, double* data, int* lda, int* ipiv, double* work, int* lwork, int* info)
{
	NOTIMP;
}

void getri(int* M, float* data, int* lda, int* ipiv, float* work, int* lwork, int* info)
{
	NOTIMP;
}

#endif

} // namespace lapack
} // namespace maths
} // namespace eckit

#endif
