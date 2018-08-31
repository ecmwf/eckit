# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

ecbuild_add_test( TARGET   eckit_test_linalg_factory
                  ARGS     --log_level=message
                  SOURCES  test_la_factory.cc
                  LIBS     eckit_linalg )

ecbuild_add_test( TARGET   eckit_test_linalg_linalg
                  ARGS     --log_level=message
                  SOURCES  test_la_linalg.cc util.h
                  LIBS     eckit_linalg )

ecbuild_add_test( TARGET     eckit_test_linalg_linalg_armadillo
                  COMMAND    eckit_test_linalg_linalg
                  CONDITION  ECKIT_HAVE_ARMADILLO
                  ARGS       --log_level=message -linearAlgebraBackend armadillo )

# This test seems to have a system call exit with 1 even though tests pass.
# Ignore system errors, see also http://stackoverflow.com/a/20360334/396967
ecbuild_add_test( TARGET     eckit_test_linalg_linalg_cuda
                  COMMAND    eckit_test_linalg_linalg
                  CONDITION  ECKIT_HAVE_CUDA
                  ARGS       --log_level=message --catch_system_errors=no -linearAlgebraBackend cuda )

ecbuild_add_test( TARGET     eckit_test_linalg_linalg_eigen
                  COMMAND    eckit_test_linalg_linalg
                  CONDITION  ECKIT_HAVE_EIGEN
                  ARGS       --log_level=message -linearAlgebraBackend eigen )

ecbuild_add_test( TARGET     eckit_test_linalg_linalg_lapack
                  COMMAND    eckit_test_linalg_linalg
                  CONDITION  ECKIT_HAVE_LAPACK
                  ARGS       --log_level=message -linearAlgebraBackend lapack )

ecbuild_add_test( TARGET     eckit_test_linalg_linalg_mkl
                  COMMAND    eckit_test_linalg_linalg
                  CONDITION  ECKIT_HAVE_MKL
                  ARGS       --log_level=message -linearAlgebraBackend mkl )

ecbuild_add_test( TARGET     eckit_test_linalg_linalg_viennacl
                  COMMAND    eckit_test_linalg_linalg
                  CONDITION  ECKIT_HAVE_VIENNACL
                  ARGS       --log_level=message -linearAlgebraBackend viennacl )

ecbuild_add_test( TARGET   eckit_test_linalg_sparse
                  ARGS     --log_level=message
                  SOURCES  test_la_sparse.cc util.h
                  LIBS     eckit_linalg )

ecbuild_add_test( TARGET     eckit_test_linalg_sparse_armadillo
                  COMMAND    eckit_test_linalg_sparse
                  CONDITION  ECKIT_HAVE_ARMADILLO
                  ARGS       --log_level=message -linearAlgebraBackend armadillo )

# This test seems to have a system call exit with 1 even though tests pass.
# Ignore system errors, see also http://stackoverflow.com/a/20360334/396967
ecbuild_add_test( TARGET     eckit_test_linalg_sparse_cuda
                  COMMAND    eckit_test_linalg_sparse
                  CONDITION  ECKIT_HAVE_CUDA
                  ARGS       --log_level=message --catch_system_errors=no -linearAlgebraBackend cuda )

ecbuild_add_test( TARGET     eckit_test_linalg_sparse_eigen
                  COMMAND    eckit_test_linalg_sparse
                  CONDITION  ECKIT_HAVE_EIGEN
                  ARGS       --log_level=message -linearAlgebraBackend eigen )

ecbuild_add_test( TARGET     eckit_test_linalg_sparse_lapack
                  COMMAND    eckit_test_linalg_sparse
                  CONDITION  ECKIT_HAVE_LAPACK
                  ARGS       --log_level=message -linearAlgebraBackend lapack )

ecbuild_add_test( TARGET     eckit_test_linalg_sparse_mkl
                  COMMAND    eckit_test_linalg_sparse
                  CONDITION  ECKIT_HAVE_MKL
                  ARGS       --log_level=message -linearAlgebraBackend mkl )

ecbuild_add_test( TARGET     eckit_test_linalg_sparse_viennacl
                  COMMAND    eckit_test_linalg_sparse
                  CONDITION  ECKIT_HAVE_VIENNACL
                  ARGS       --log_level=message -linearAlgebraBackend viennacl )

ecbuild_add_test( TARGET   eckit_test_linalg_streaming
                  ARGS     --log_level=message
                  SOURCES  test_la_streaming.cc util.h
                  LIBS     eckit_linalg )
