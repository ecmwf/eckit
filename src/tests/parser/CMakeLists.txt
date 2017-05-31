ecbuild_add_test( TARGET   eckit_test_json
                  BOOST
                  SOURCES  test_json.cc
                  LIBS     eckit )

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/foo.yaml
    "
    # begin
    { \"a\" : [true, false, 3],
    \"b\" : 42.3 ,
    \"c\" : null,
    # this is a comment
    \"d\" : \"foo\tbar\",
    \"e\" : \"769b654c42b759e3604e4fa37f0e721dc4797818\"}
    # end
    ")


ecbuild_add_test( TARGET   eckit_test_yaml
                  BOOST
                  SOURCES  test_yaml.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_json_metadata
                  BOOST
                  SOURCES  test_json_metadata.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_stream_parser
                  BOOST
                  SOURCES  test_stream_parser.cc
                  LIBS     eckit )
