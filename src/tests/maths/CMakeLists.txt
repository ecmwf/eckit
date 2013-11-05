ecbuild_add_test(   TARGET      test_maths_expr
                    SOURCES     test_maths_expr.cc
                    CONDITION   ECKIT_MATHS
                    LIBS        eckit_maths eckit )

