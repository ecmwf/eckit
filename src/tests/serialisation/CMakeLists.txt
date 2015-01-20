ecbuild_add_test( TARGET   test_file_stream
                  SOURCES  test_file_stream.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   test_streamable
                  ARGS --log_level=message
                  BOOST
                  SOURCES  test_streamable.cc
                  LIBS     eckit )
