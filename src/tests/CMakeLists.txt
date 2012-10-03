add_subdirectory( logging  )
add_subdirectory( config   )

ecbuild_add_test(   TARGET      test_tokenizer
                    SOURCES     test_tokenizer.cc
                    LIBS        Ec )
