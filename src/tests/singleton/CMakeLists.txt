ecbuild_add_library( TARGET      testfactory
                     NOINSTALL
                     SOURCES     TestFactory.cc TestFactory.h )

ecbuild_add_library( TARGET      testbuilder1
                     NOINSTALL
                     SOURCES     TestBuilder1.cc
                     LIBS        testfactory )

ecbuild_add_library( TARGET      testbuilder2
                     NOINSTALL
                     SOURCES     TestBuilder2.cc
                     LIBS        testfactory )

ecbuild_add_test( TARGET      test_singleton
                  SOURCES     test_singleton.cc
                  LIBS        testbuilder1 testbuilder2 )
