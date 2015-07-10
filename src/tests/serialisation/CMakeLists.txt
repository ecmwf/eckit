ecbuild_add_test( TARGET   eckit_test_file_stream
                  ARGS     --log_level=message
                  BOOST
                  SOURCES  test_file_stream.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_streamable
                  ARGS     --log_level=message
                  BOOST
                  SOURCES  test_streamable.cc
                  LIBS     eckit )
