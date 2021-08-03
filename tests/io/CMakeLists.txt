### test data files
list( APPEND test_data_files
t.grib
)

if(HAVE_EXTRA_TESTS)
    ecbuild_get_test_multidata( TARGET get_eckit_io_test_data
                                NAMES ${test_data_files}
                                NOCHECK )
endif()

ecbuild_add_test( TARGET   eckit_test_filepool
                  SOURCES  test_filepool.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET  eckit_test_urlhandle
                  SOURCES test_urlhandle.cc
                  CONDITION HAVE_EXTRA_TESTS AND eckit_HAVE_CURL
                  LIBS    eckit )

ecbuild_add_test( TARGET  eckit_test_circularbuffer
                  SOURCES test_circularbuffer.cc
                  LIBS    eckit )

ecbuild_add_test( TARGET      eckit_test_compress
                  SOURCES     test_compress.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_bitio
                  SOURCES     test_bitio.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_base64
                  SOURCES     test_base64.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_multihandle
                  SOURCES     test_multihandle.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_partfilehandle
                  SOURCES     test_partfilehandle.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_pooledfile
                  SOURCES     test_pooledfile.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_pooledhandle
                  SOURCES     test_pooledhandle.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_buffer
                  SOURCES     test_buffer.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_bufferlist
                  SOURCES     test_bufferlist.cc
                  LIBS        eckit )

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
