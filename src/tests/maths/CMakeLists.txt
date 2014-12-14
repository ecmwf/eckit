ecbuild_add_test(   TARGET      test_eigen
					BOOST
					SOURCES     test_eigen.cc
					CONDITION   HAVE_EIGEN
					LIBS        eckit_maths eckit
)
