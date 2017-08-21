ecbuild_add_test( TARGET   eckit_test_filepool
                  BOOST
                  SOURCES  test_filepool.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET  eckit_test_datablob
                  BOOST
                  SOURCES test_datablob.cc
                  LIBS    eckit )

ecbuild_add_test( TARGET  eckit_test_urlhandle
                  BOOST
                  SOURCES test_urlhandle.cc
                  LIBS    eckit )

ecbuild_add_test( TARGET  eckit_test_circularbuffer
                  BOOST
                  SOURCES test_circularbuffer.cc
                  LIBS    eckit )

ecbuild_add_test( TARGET      eckit_test_compress
                  BOOST
                  SOURCES     test_compress.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_bitio
                  BOOST
                  SOURCES     test_bitio.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_base64
                  BOOST
                  SOURCES     test_base64.cc
                  LIBS        eckit )
