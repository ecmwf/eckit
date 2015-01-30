ecbuild_add_test(   TARGET      test_xpr_expr
					BOOST
					CONDITION   HAVE_XPR
					SOURCES     test_xpr_expr.cc
					LIBS        eckit_xpr eckit )

ecbuild_add_test(   TARGET      test_xpr_ycombinator
					BOOST
					CONDITION   HAVE_XPR
					SOURCES     test_xpr_ycombinator.cc
					LIBS        eckit_xpr eckit )

ecbuild_add_test(   TARGET      test_xpr_currying
					BOOST
					CONDITION   HAVE_XPR
					SOURCES     test_xpr_currying.cc
					LIBS        eckit_xpr eckit )

ecbuild_add_test(   TARGET      test_xpr_optimiser
					BOOST
					CONDITION   HAVE_XPR
					SOURCES     test_xpr_optimiser.cc
					LIBS        eckit_xpr eckit )

ecbuild_add_test(   TARGET      test_xpr_lambda
					BOOST
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


