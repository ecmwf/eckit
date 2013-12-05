ecbuild_add_test(   TARGET      test_latlon
					SOURCES     test_latlon.cc
					CONDITION	ENABLE_GRID
					LIBS        eckit eckit_grid )

ecbuild_add_test(   TARGET      test_gaussian
					SOURCES     test_gaussian.cc
					CONDITION	ENABLE_GRID
					LIBS        eckit eckit_grid )

ecbuild_add_test(   TARGET      test_field
					SOURCES     test_field.cc
					CONDITION	ENABLE_GRID
					LIBS        eckit eckit_grid )

				#ecbuild_add_test(   TARGET      test_action
				#		SOURCES     test_action.cc
				#		CONDITION	ENABLE_GRID
				#		LIBS        eckit eckit_grid )
