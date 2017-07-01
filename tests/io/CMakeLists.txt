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