# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

### test data files
list( APPEND test_data_files
t.grib
)

if(HAVE_EXTRA_TESTS)
    ecbuild_get_test_multidata( TARGET get_eckit_io_test_data
                                NAMES ${test_data_files}
                                NOCHECK )
endif()

ecbuild_add_test( TARGET  eckit_test_easycurl
                  SOURCES test_easycurl.cc
                  CONDITION HAVE_EXTRA_TESTS AND eckit_HAVE_CURL
                  LIBS    eckit )

ecbuild_add_test( TARGET    eckit_test_easycurl_redirect
                  SOURCES   test_easycurl_redirect.cc
                  CONDITION eckit_HAVE_CURL
                  LIBS      eckit )

file(
  COPY mock/MockREST.py mock/MockREST.launcher.sh
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

ecbuild_add_test( TARGET  eckit_test_urlhandle
                  SOURCES test_urlhandle.cc
                  CONDITION HAVE_EXTRA_TESTS AND eckit_HAVE_CURL
                  LIBS    eckit )

foreach(_test
        base64
        bitio
        buffer
        bufferlist
        circularbuffer
        compress
        filelock
        filepool
        multihandle
        partfilehandle
        pooledfile
        pooledhandle )
  ecbuild_add_test( TARGET  eckit_test_${_test}
                    SOURCES test_${_test}.cc
                    LIBS    eckit )
endforeach()

# The RADOS tests need a cluster. Unless they manage their own pools (ENABLE_RADOS_TESTS_MANAGE_POOLS)
# they work in an existing pool, named by the ECKIT_RADOS_TEST_POOL cmake variable or, if that is not
# set, by the ECKIT_RADOS_TEST_POOL environment variable at the time the tests run.
set( _rados_test_env )
if( eckit_HAVE_RADOS AND NOT eckit_HAVE_RADOS_TESTS_MANAGE_POOLS )
    if( ECKIT_RADOS_TEST_POOL )
        list( APPEND _rados_test_env ECKIT_RADOS_TEST_POOL=${ECKIT_RADOS_TEST_POOL} )
    else()
        message( STATUS "RADOS tests: ECKIT_RADOS_TEST_POOL is not set, the tests will read it from the environment" )
    endif()
endif()

ecbuild_add_test( TARGET      eckit_test_radoshandle
                  SOURCES     test_radoshandle.cc
                  CONDITION   eckit_HAVE_RADOS
                  LABELS      rados
                  LIBS        eckit
                  ENVIRONMENT ${_rados_test_env} )

ecbuild_add_test( TARGET      eckit_test_rados_kv
                  SOURCES     test_rados_kv.cc
                  CONDITION   eckit_HAVE_RADOS
                  LABELS      rados
                  LIBS        eckit
                  ENVIRONMENT ${_rados_test_env} )

ecbuild_add_test( TARGET      eckit_rados-performance
                  SOURCES     rados-performance.cc
                  CONDITION   HAVE_EXTRA_TESTS AND eckit_HAVE_RADOS
                  LABELS      rados
                  TEST_DEPENDS get_eckit_io_test_data
                  LIBS        eckit
                  ENVIRONMENT ${_rados_test_env} )

add_subdirectory( fam )
