ecbuild_add_test( TARGET   eckit_test_parser_json
                  SOURCES  test_json.cc
                  LIBS     eckit )

ecbuild_add_executable( TARGET   eckit_yaml_file
                        SOURCES  eckit_yaml_file.cc
                        LIBS     eckit_option
                        NOINSTALL )

function(test_yaml_file test_file check_file)
    if(NOT DEFINED check_file)
        ecbuild_add_test( TARGET eckit_test_yaml_file_${test_file}
                          COMMAND           eckit_yaml_file
                          ARGS              ${test_file} --check-file=${check_file}
                          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
    else()
        ecbuild_add_test( TARGET eckit_test_yaml_file_${test_file}
                          COMMAND           eckit_yaml_file
                          ARGS              ${test_file}
                          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
    endif()
endfunction()

test_yaml_file( 2.1.yaml 2.1.json )
test_yaml_file( 2.2.yaml 2.2.json )
test_yaml_file( 2.3.yaml 2.3.json )
test_yaml_file( 2.4.yaml 2.4.json )
test_yaml_file( 2.5.yaml 2.5.json )
test_yaml_file( 2.6.yaml 2.6.json )
test_yaml_file( 2.7.yaml 2.7.json )
test_yaml_file( 2.8.yaml 2.8.json )
test_yaml_file( 2.9.yaml 2.9.json )
test_yaml_file( 2.10.yaml 2.10.json )
test_yaml_file( 2.11.yaml )  # not decoded properly
test_yaml_file( 2.12.yaml 2.12.json )
test_yaml_file( 2.13.yaml 2.13.json )
test_yaml_file( 2.14.yaml 2.14.json )
test_yaml_file( 2.15.yaml 2.15.json )
test_yaml_file( 2.16.yaml )  # not decoded properly
# test_yaml_file( 2.17.yaml 2.17.json )  # cannot decode
test_yaml_file( 2.18.yaml )  # not decoded properly
test_yaml_file( 2.19.yaml 2.19.json )
test_yaml_file( 2.20.yaml 2.20.json )
test_yaml_file( 2.21.yaml 2.21.json )
test_yaml_file( 2.22.yaml )  # not decoded properly
test_yaml_file( 2.23.yaml )  # not decoded properly
# test_yaml_file( 2.24.yaml 2.24.json )  # cannot decode
test_yaml_file( 2.25.yaml )  # not decoded properly
test_yaml_file( 2.26.yaml )  # not decoded properly
test_yaml_file( 2.27.yaml )  # not decoded properly
test_yaml_file( 2.28.yaml )  # not decoded properly

unset( test_yaml_file )

ecbuild_add_test( TARGET            eckit_test_parser_yaml
                  SOURCES           test_yaml.cc
                  LIBS              eckit
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

ecbuild_add_test( TARGET   eckit_test_parser_stream_parser
                  SOURCES  test_stream_parser.cc
                  LIBS     eckit )

ecbuild_add_test( TARGET   eckit_test_parser_csv
                  SOURCES  test_csv.cc
                  LIBS     eckit )
