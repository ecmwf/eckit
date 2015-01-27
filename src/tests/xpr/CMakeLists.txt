ecbuild_add_test(   TARGET      test_xpr_expr
					CONDITION   HAVE_XPR
					SOURCES     test_xpr_expr.cc
					LIBS        eckit_xpr eckit )

ecbuild_add_test(   TARGET      test_maths_ycombinator
					CONDITION   HAVE_XPR
					SOURCES     test_xpr_ycombinator.cc
					LIBS        eckit_xpr eckit )

ecbuild_add_test(   TARGET      test_xpr_currying
					CONDITION   HAVE_XPR
					SOURCES     test_xpr_currying.cc
					LIBS        eckit_xpr eckit )

ecbuild_add_test(   TARGET      test_xpr_optimiser
					CONDITION   HAVE_XPR
					SOURCES     test_xpr_optimiser.cc
					LIBS        eckit_xpr eckit )

ecbuild_add_test(   TARGET      test_xpr_lambda
					CONDITION   HAVE_XPR
					SOURCES     test_xpr_lambda.cc
					LIBS        eckit_xpr eckit )

ecbuild_add_test(   TARGET      test_xpr_streaming
					BOOST
					CONDITION   HAVE_XPR
					SOURCES     test_xpr_streaming.cc
					LIBS        eckit_xpr eckit )

ecbuild_add_test(   TARGET      test_xpr_usage
					BOOST
					CONDITION   HAVE_XPR
					SOURCES     test_xpr_usage.cc
					LIBS        eckit_xpr eckit )


