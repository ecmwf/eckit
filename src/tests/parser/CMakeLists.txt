ecbuild_add_test( TARGET   eckit_test_json
                  BOOST
                  SOURCES  test_json.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_json_metadata
                  BOOST
                  SOURCES  test_json_metadata.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_stream_parser
                  BOOST
                  SOURCES  test_stream_parser.cc
                  LIBS     eckit )
