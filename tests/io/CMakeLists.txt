ecbuild_add_test( TARGET   eckit_test_filepool
                  SOURCES  test_filepool.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET  eckit_test_datablob
                  SOURCES test_datablob.cc
                  LIBS    eckit )

ecbuild_add_test( TARGET  eckit_test_urlhandle
                  SOURCES test_urlhandle.cc
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
