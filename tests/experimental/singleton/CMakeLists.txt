ecbuild_append_to_rpath( "./" )

ecbuild_add_library( TARGET      eckit_test_experimental_singleton_factory
                     NOINSTALL
                     SOURCES     TestFactory.cc TestFactory.h
                     LIBS        eckit )

ecbuild_add_library( TARGET      eckit_test_experimental_singleton_builder1
                     NOINSTALL
                     SOURCES     TestBuilder1.cc
                     LIBS        eckit_test_experimental_singleton_factory )

ecbuild_add_library( TARGET      eckit_test_experimental_singleton_builder2
                     NOINSTALL
                     SOURCES     TestBuilder2.cc
                     LIBS        eckit_test_experimental_singleton_factory )

ecbuild_add_test( TARGET      eckit_test_experimental_singleton_singleton
                  SOURCES     test_singleton.cc
                  LIBS        eckit_test_builder1 eckit_test_builder2 )
