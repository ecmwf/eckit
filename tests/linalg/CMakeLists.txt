# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

ecbuild_add_test( TARGET    eckit_test_linalg_factory
                  ARGS      --log_level=message
                  SOURCES   test_la_factory.cc
                  LIBS      eckit_linalg )

ecbuild_add_test( TARGET    eckit_test_linalg_dense_backend
                  ARGS      --log_level=message
                  SOURCES   test_la_linalg_dense.cc util.h
                  LIBS      eckit_linalg )

ecbuild_add_test( TARGET    eckit_test_linalg_sparse_backend
                  ARGS      --log_level=message
                  SOURCES   test_la_linalg_sparse.cc util.h
                  LIBS      eckit_linalg )

ecbuild_add_test( TARGET    eckit_test_linalg_matrix
                  ARGS      --log_level=message
                  SOURCES   test_la_matrix.cc
                  LIBS      eckit_linalg )

ecbuild_add_test( TARGET    eckit_test_linalg_tensor
                  ARGS      --log_level=message
                  SOURCES   test_la_tensor.cc util.h
                  LIBS      eckit_linalg )

# LinearAlgebra
ecbuild_add_test( TARGET    eckit_test_linalg_mixed_backend
                  ARGS      --log_level=message
                  SOURCES   test_la_linalg.cc util.h
                  LIBS      eckit_linalg )

ecbuild_add_test( TARGET    eckit_test_linalg_mixed_backend_generic
                  COMMAND   eckit_test_linalg_mixed_backend
                  ARGS      --log_level=message -linearAlgebraBackend generic )

ecbuild_add_test( TARGET    eckit_test_linalg_mixed_backend_cuda
                  COMMAND   eckit_test_linalg_mixed_backend
                  CONDITION eckit_HAVE_CUDA
                  ARGS      --log_level=message -linearAlgebraBackend cuda --catch_system_errors=no )

ecbuild_add_test( TARGET    eckit_test_linalg_mixed_backend_eigen
                  COMMAND   eckit_test_linalg_mixed_backend
                  CONDITION eckit_HAVE_EIGEN
                  ARGS      --log_level=message -linearAlgebraBackend eigen )

ecbuild_add_test( TARGET    eckit_test_linalg_mixed_backend_lapack
                  COMMAND   eckit_test_linalg_mixed_backend
                  CONDITION eckit_HAVE_LAPACK
                  ARGS      --log_level=message -linearAlgebraBackend lapack )

ecbuild_add_test( TARGET    eckit_test_linalg_mixed_backend_mkl
                  COMMAND   eckit_test_linalg_mixed_backend
                  CONDITION eckit_HAVE_MKL
                  ARGS      --log_level=message -linearAlgebraBackend mkl )

ecbuild_add_test( TARGET    eckit_test_linalg_mixed_backend_openmp
                  COMMAND   eckit_test_linalg_mixed_backend
                  CONDITION eckit_HAVE_OMP
                  ARGS      --log_level=message -linearAlgebraBackend openmp )

# LinearAlgebraDense

ecbuild_add_test( TARGET    eckit_test_linalg_dense_backend_generic
                  COMMAND   eckit_test_linalg_dense_backend
                  ARGS      --log_level=message -linearAlgebraDenseBackend generic )

# This test seems to have a system call exit with 1 even though tests pass.
# Ignore system errors, see also http://stackoverflow.com/a/20360334/396967
ecbuild_add_test( TARGET    eckit_test_linalg_dense_backend_cuda
                  COMMAND   eckit_test_linalg_dense_backend
                  CONDITION eckit_HAVE_CUDA
                  ARGS      --log_level=message -linearAlgebraDenseBackend cuda --catch_system_errors=no )

ecbuild_add_test( TARGET    eckit_test_linalg_dense_backend_eigen
                  COMMAND   eckit_test_linalg_dense_backend
                  CONDITION eckit_HAVE_EIGEN
                  ARGS      --log_level=message -linearAlgebraDenseBackend eigen )

ecbuild_add_test( TARGET    eckit_test_linalg_dense_backend_lapack
                  COMMAND   eckit_test_linalg_dense_backend
                  CONDITION eckit_HAVE_LAPACK
                  ARGS      --log_level=message -linearAlgebraDenseBackend lapack )

ecbuild_add_test( TARGET    eckit_test_linalg_dense_backend_mkl
                  COMMAND   eckit_test_linalg_dense_backend
                  CONDITION eckit_HAVE_MKL
                  ARGS      --log_level=message -linearAlgebraDenseBackend mkl )

ecbuild_add_test( TARGET    eckit_test_linalg_dense_backend_torch
                  COMMAND   eckit_test_linalg_dense_backend
                  CONDITION eckit_HAVE_TORCH
                  ARGS      --log_level=message -linearAlgebraDenseBackend torch )

ecbuild_add_test( TARGET    eckit_test_linalg_dense_backend_openmp
                  COMMAND   eckit_test_linalg_dense_backend
                  CONDITION eckit_HAVE_OMP
                  ARGS      --log_level=message -linearAlgebraDenseBackend openmp )

# LinearAlgebraSparse

ecbuild_add_test( TARGET    eckit_test_linalg_sparse_backend_generic
                  COMMAND   eckit_test_linalg_sparse_backend
                  ARGS      --log_level=message -linearAlgebraSparseBackend generic )

# This test seems to have a system call exit with 1 even though tests pass.
# Ignore system errors, see also http://stackoverflow.com/a/20360334/396967
ecbuild_add_test( TARGET    eckit_test_linalg_sparse_backend_cuda
                  COMMAND   eckit_test_linalg_sparse_backend
                  CONDITION eckit_HAVE_CUDA
                  ARGS      --log_level=message -linearAlgebraSparseBackend cuda --catch_system_errors=no )

ecbuild_add_test( TARGET    eckit_test_linalg_sparse_backend_hip
                  COMMAND   eckit_test_linalg_sparse_backend
                  CONDITION eckit_HAVE_HIP
                  ARGS      --log_level=message -linearAlgebraSparseBackend hip )

ecbuild_add_test( TARGET    eckit_test_linalg_sparse_backend_eigen
                  COMMAND   eckit_test_linalg_sparse_backend
                  CONDITION eckit_HAVE_EIGEN
                  ARGS      --log_level=message -linearAlgebraSparseBackend eigen )

ecbuild_add_test( TARGET    eckit_test_linalg_sparse_backend_mkl
                  COMMAND   eckit_test_linalg_sparse_backend
                  CONDITION eckit_HAVE_MKL
                  ARGS      --log_level=message -linearAlgebraSparseBackend mkl )

ecbuild_add_test( TARGET    eckit_test_linalg_sparse_backend_openmp
                  COMMAND   eckit_test_linalg_sparse_backend
                  CONDITION eckit_HAVE_OMP
                  ARGS      --log_level=message -linearAlgebraSparseBackend openmp )

ecbuild_add_test( TARGET    eckit_test_linalg_sparse_backend_torch
                  COMMAND   eckit_test_linalg_sparse_backend
                  CONDITION eckit_HAVE_TORCH
                  ARGS      --log_level=message -linearAlgebraSparseBackend torch )

#

ecbuild_add_test( TARGET    eckit_test_linalg_sparse
                  ARGS      --log_level=message
                  SOURCES   test_la_sparse.cc util.h
                  LIBS      eckit_linalg )

ecbuild_add_test( TARGET    eckit_test_linalg_streaming
                  ARGS      --log_level=message
                  SOURCES   test_la_streaming.cc util.h
                  LIBS      eckit_linalg )
