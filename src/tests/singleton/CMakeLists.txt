ecbuild_append_to_rpath( "./" )

ecbuild_add_library( TARGET      test_factory
                     NOINSTALL
                     SOURCES     TestFactory.cc TestFactory.h
                     LIBS        eckit )

ecbuild_add_library( TARGET      test_builder1
                     NOINSTALL
                     SOURCES     TestBuilder1.cc
                     LIBS        test_factory )

ecbuild_add_library( TARGET      test_builder2
                     NOINSTALL
                     SOURCES     TestBuilder2.cc
                     LIBS        test_factory )

ecbuild_add_test( TARGET      eckit_test_singleton
                  SOURCES     test_singleton.cc
                  LIBS        test_builder1 test_builder2 )
