ecbuild_add_test(   TARGET      test_maths_expr
					CONDITION   HAVE_MATHS
					SOURCES     test_maths_expr.cc
					LIBS        eckit_maths eckit )

ecbuild_add_test(   TARGET      test_maths_ycombinator
					CONDITION   HAVE_MATHS
					SOURCES     test_maths_ycombinator.cc
					LIBS        eckit_maths eckit )

ecbuild_add_test(   TARGET      test_maths_currying
					CONDITION   HAVE_MATHS
					SOURCES     test_maths_currying.cc
					LIBS        eckit_maths eckit )

ecbuild_add_test(   TARGET      test_maths_optimiser
					CONDITION   HAVE_MATHS
					SOURCES     test_maths_optimiser.cc
					LIBS        eckit_maths eckit )

ecbuild_add_test(   TARGET      test_maths_lambda
					CONDITION   HAVE_MATHS
					SOURCES     test_maths_lambda.cc
					LIBS        eckit_maths eckit )


