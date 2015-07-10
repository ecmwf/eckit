# (C) Copyright 1996-2015 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

ecbuild_add_test( TARGET   eckit_test_la_factory
                  ARGS     --log_level=message
                  BOOST
                  SOURCES  test_la_factory.cc
                  LIBS     eckit_la )

ecbuild_add_test( TARGET   eckit_test_la_linalg
                  ARGS     --log_level=message
                  BOOST
                  SOURCES  test_la_linalg.cc util.h
                  LIBS     eckit_la )

ecbuild_add_test( TARGET     eckit_test_la_linalg_armadillo
                  COMMAND    test_la_linalg
                  CONDITION  ECKIT_HAVE_ARMADILLO
                  ARGS       --log_level=message -linearAlgebraBackend armadillo )

# This test seems to have a system call exit with 1 even though tests pass.
# Ignore system errors, see also http://stackoverflow.com/a/20360334/396967
ecbuild_add_test( TARGET     eckit_test_la_linalg_cuda
                  COMMAND    test_la_linalg
                  CONDITION  ECKIT_HAVE_CUDA
                  ARGS       --log_level=message --catch_system_errors=no -linearAlgebraBackend cuda )

ecbuild_add_test( TARGET     eckit_test_la_linalg_eigen
                  COMMAND    test_la_linalg
                  CONDITION  ECKIT_HAVE_EIGEN
                  ARGS       --log_level=message -linearAlgebraBackend eigen )

ecbuild_add_test( TARGET     eckit_test_la_linalg_mkl
                  COMMAND    test_la_linalg
                  CONDITION  ECKIT_HAVE_MKL
                  ARGS       --log_level=message -linearAlgebraBackend mkl )

ecbuild_add_test( TARGET   eckit_test_la_sparse
                  ARGS     --log_level=message
                  BOOST
                  SOURCES  test_la_sparse.cc util.h
                  LIBS     eckit_la )

# This test seems to have a system call exit with 1 even though tests pass.
# Ignore system errors, see also http://stackoverflow.com/a/20360334/396967
ecbuild_add_test( TARGET     eckit_test_la_sparse_cuda
                  COMMAND    test_la_sparse
                  CONDITION  ECKIT_HAVE_CUDA
                  ARGS       --log_level=message --catch_system_errors=no -linearAlgebraBackend cuda )

ecbuild_add_test( TARGET     eckit_test_la_sparse_eigen
                  COMMAND    test_la_sparse
                  CONDITION  ECKIT_HAVE_EIGEN
                  ARGS       --log_level=message -linearAlgebraBackend eigen )

ecbuild_add_test( TARGET     eckit_test_la_sparse_mkl
                  COMMAND    test_la_sparse
                  CONDITION  ECKIT_HAVE_MKL
                  ARGS       --log_level=message -linearAlgebraBackend mkl )

ecbuild_add_test( TARGET   eckit_test_la_streaming
                  ARGS     --log_level=message
                  BOOST
                  SOURCES  test_la_streaming.cc util.h
                  LIBS     eckit_la )
