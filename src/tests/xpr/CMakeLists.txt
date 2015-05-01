ecbuild_get_cxx11_flags( CXX11_FLAGS )

find_package(PythonInterp)
if(PYTHONINTERP_FOUND)
  ADD_CUSTOM_COMMAND(
    OUTPUT test_xpr_operators.cc
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gen_xpr_tests.py ${CMAKE_CURRENT_SOURCE_DIR}/test_xpr_operators.cc.in test_xpr_operators.cc
    DEPENDS gen_xpr_tests.py
  )
endif()

ecbuild_add_test( TARGET      test_xpr_expr
                  BOOST
                  CONDITION   HAVE_XPR
                  SOURCES     test_xpr_expr.cc
                  CXXFLAGS    ${CXX11_FLAGS}
                  LIBS        eckit_xpr eckit )

ecbuild_add_test( TARGET      test_xpr_ycombinator
                  BOOST
                  CONDITION   HAVE_XPR
                  SOURCES     test_xpr_ycombinator.cc
                  CXXFLAGS    ${CXX11_FLAGS}
                  LIBS        eckit_xpr eckit )

ecbuild_add_test( TARGET      test_xpr_currying
                  BOOST
                  CONDITION   HAVE_XPR
                  SOURCES     test_xpr_currying.cc
                  CXXFLAGS    ${CXX11_FLAGS}
                  LIBS        eckit_xpr eckit )

ecbuild_add_test( TARGET      test_xpr_operators
                  BOOST
                  CONDITION   HAVE_XPR AND PYTHONINTERP_FOUND
                  SOURCES     test_xpr_operators.cc
                  CXXFLAGS    ${CXX11_FLAGS}
                  LIBS        eckit_xpr eckit )

ecbuild_add_test( TARGET      test_xpr_optimiser
                  BOOST
                  CONDITION   HAVE_XPR
                  SOURCES     test_xpr_optimiser.cc
                  CXXFLAGS    ${CXX11_FLAGS}
                  LIBS        eckit_xpr eckit )

ecbuild_add_test( TARGET      test_xpr_lambda
                  BOOST
                  CONDITION   HAVE_XPR
                  SOURCES     test_xpr_lambda.cc
                  CXXFLAGS    ${CXX11_FLAGS}
                  LIBS        eckit_xpr eckit )

ecbuild_add_test( TARGET      test_xpr_foreach
                  BOOST
                  CONDITION   HAVE_XPR
                  SOURCES     test_xpr_foreach.cc
                  CXXFLAGS    ${CXX11_FLAGS}
                  LIBS        eckit_xpr eckit )

ecbuild_add_test( TARGET      test_xpr_linalg
                  BOOST
                  CONDITION   HAVE_XPR
                  SOURCES     test_xpr_linalg.cc
                  CXXFLAGS    ${CXX11_FLAGS}
                  LIBS        eckit_xpr eckit )

ecbuild_add_test( TARGET      test_xpr_streaming
                  BOOST
                  CONDITION   HAVE_XPR
                  SOURCES     test_xpr_streaming.cc
                  CXXFLAGS    ${CXX11_FLAGS}
                  LIBS        eckit_xpr eckit )

ecbuild_add_test( TARGET      test_xpr_unary
                  BOOST
                  CONDITION   HAVE_XPR
                  SOURCES     test_xpr_unary.cc
                  CXXFLAGS    ${CXX11_FLAGS}
                  LIBS        eckit_xpr eckit )

ecbuild_add_test( TARGET      test_xpr_usage
                  BOOST
                  CONDITION   HAVE_XPR
                  SOURCES     test_xpr_usage.cc
                  CXXFLAGS    ${CXX11_FLAGS}
                  LIBS        eckit_xpr eckit )
