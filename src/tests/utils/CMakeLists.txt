ecbuild_add_test( TARGET      eckit_test_string_tools
                  SOURCES     test_string_tools.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_translator
                  BOOST
                  SOURCES     test_translator.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_tokenizer
                  SOURCES     test_tokenizer.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_utils
                  BOOST
                  SOURCES     test_md5.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_sha1
                  BOOST
                  CONDITION   ECKIT_HAVE_SSL
                  SOURCES     test_sha1.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_md4
                              BOOST
                              CONDITION   ECKIT_HAVE_SSL
                              SOURCES     test_md4.cc
                              LIBS        eckit )

ecbuild_add_test( TARGET      hash-performance
                  CONDITION   ECKIT_HAVE_SSL
                  SOURCES     hash-performance.cc
                  LIBS        eckit )
