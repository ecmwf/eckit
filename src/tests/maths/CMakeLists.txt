ecbuild_add_test(   TARGET      test_maths_expr
					CONDITION   HAVE_MATHS AND Boost_FOUND
					SOURCES     test_maths_expr.cc
					INCLUDES    ${Boost_INCLUDE_DIRS}
					LIBS        eckit_maths eckit )

