ecbuild_add_test( TARGET   eckit_test_serialisation_file_stream
                  SOURCES  test_file_stream.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_serialisation_streamable
                  SOURCES  test_streamable.cc
                  LIBS     eckit )
