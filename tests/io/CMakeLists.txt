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

ecbuild_add_test( TARGET      eckit_test_radoshandle
                  SOURCES     test_radoshandle.cc
                  CONDITION   HAVE_RADOS
                  INCLUDES    ${RADOS_INCLUDE_DIRS}
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_rados-performance
                  SOURCES     rados-performance.cc
                  CONDITION   HAVE_EXTRA_TESTS AND HAVE_RADOS
                  INCLUDES    ${RADOS_INCLUDE_DIRS}
                  TEST_DEPENDS get_eckit_io_test_data
                  LIBS        eckit )
