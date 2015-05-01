ecbuild_add_test(   TARGET      test_string_tools
					SOURCES     test_string_tools.cc
					LIBS        eckit )

ecbuild_add_test(   TARGET      test_translator
					BOOST
					SOURCES     test_translator.cc
					LIBS        eckit )

ecbuild_add_test(   TARGET      test_tokenizer
                    SOURCES     test_tokenizer.cc
                    LIBS        eckit )

ecbuild_add_test(   TARGET      test_eckit_utils
					BOOST
                    SOURCES     test_md5.cc
                    LIBS        eckit )
