#ifndef eckit_maths_lapack_h
#define eckit_maths_lapack_h

#include "eckit/eckit.h"

namespace eckit::maths::lapack {

void getrf(int* M, int* N, float* data, int* lda, int* ipiv, int* info);
void getrf(int* M, int* N, double* data, int* lda, int* ipiv, int* info);

void getri(int* M, float* data, int* lda, int* ipiv, float* work, int* lwork, int* info);
void getri(int* M, double* data, int* lda, int* ipiv, double* work, int* lwork, int* info);

}  // namespace eckit::maths::lapack

#endif
