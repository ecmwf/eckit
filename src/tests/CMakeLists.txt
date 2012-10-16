add_subdirectory( logging  )
add_subdirectory( config   )

ecbuild_add_test(   TARGET      test_tokenizer
                    SOURCES     test_tokenizer.cc
                    LIBS        Ec )

ecbuild_add_test(   TARGET      test_resource
                    SOURCES     test_resource.cc
                    ARGS        -integer 100
                    LIBS        Ec )

ecbuild_add_test(   TARGET      test_string_tools
                    SOURCES     test_string_tools.cc
                    LIBS        Ec )
