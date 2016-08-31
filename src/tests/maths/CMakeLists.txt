ecbuild_add_test( TARGET      eckit_test_eigen
                  BOOST
                  SOURCES     test_eigen.cc
                  CONDITION   HAVE_EIGEN
                  LIBS        eckit_maths eckit
)
ecbuild_add_test( TARGET      eckit_test_matrix_lapack
                  BOOST
                  SOURCES     test_matrix.cc
                  CONDITION   HAVE_LAPACK AND NOT HAVE_EIGEN
                  LIBS        eckit_maths eckit
)

