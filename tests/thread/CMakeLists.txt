
# This test is causing problems for Intel 19+ compilers
# so skip it for now until it is better understood
if( NOT CMAKE_CXX_COMPILER_ID MATCHES Intel OR CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19)
    ecbuild_add_test( TARGET      eckit_test_thread_mutex
                      SOURCES     test_mutex.cc
                      LIBS        eckit )
endif()