find_package(PythonInterp)
if(PYTHONINTERP_FOUND)
  ADD_CUSTOM_COMMAND(
    OUTPUT test_xpr_operators.cc
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gen_xpr_tests.py ${CMAKE_CURRENT_SOURCE_DIR}/test_xpr_operators.cc.in test_xpr_operators.cc
    DEPENDS gen_xpr_tests.py
  )
endif()

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

ecbuild_add_test(   TARGET      test_xpr_operators
					BOOST
					CONDITION   HAVE_XPR AND PYTHONINTERP_FOUND
					SOURCES     test_xpr_operators.cc
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

ecbuild_add_test(   TARGET      test_xpr_foreach
					BOOST
					CONDITION   HAVE_XPR
					SOURCES     test_xpr_foreach.cc
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


