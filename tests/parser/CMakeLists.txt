ecbuild_add_test( TARGET   eckit_test_parser_json
                  SOURCES  test_json.cc
                  LIBS     eckit )

list( APPEND yaml
2.1.yaml 2.10.yaml 2.11.yaml 2.12.yaml 2.13.yaml 2.14.yaml 2.15.yaml 2.16.yaml 2.17.yaml
2.18.yaml 2.19.yaml 2.2.yaml 2.20.yaml 2.21.yaml 2.22.yaml 2.23.yaml 2.24.yaml 2.25.yaml
2.26.yaml 2.27.yaml 2.28.yaml 2.3.yaml 2.4.yaml 2.5.yaml 2.6.yaml 2.7.yaml 2.8.yaml 2.9.yaml
cfg.1.yaml string.yaml unicode.yaml
)

foreach( r ${yaml} )
   configure_file( ${r} ${CMAKE_CURRENT_BINARY_DIR}/${r} @ONLY )
endforeach()


ecbuild_add_test( TARGET   eckit_test_parser_yaml
                  SOURCES  test_yaml.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_parser_stream_parser
                  SOURCES  test_stream_parser.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_parser_csv
                  SOURCES  test_csv.cc
                  LIBS     eckit )
