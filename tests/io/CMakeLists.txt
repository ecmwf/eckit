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
