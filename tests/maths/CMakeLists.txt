ecbuild_add_test( TARGET      eckit_test_eigen
                  BOOST
                  SOURCES     test_eigen.cc
                  CONDITION   HAVE_EIGEN
                  LIBS        eckit_maths eckit
)
ecbuild_add_test( TARGET      eckit_test_matrix
                  BOOST
                  SOURCES     test_matrix.cc
                  LIBS        eckit_maths eckit
)
