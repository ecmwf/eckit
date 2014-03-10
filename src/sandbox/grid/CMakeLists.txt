ecbuild_add_test(
        TARGET      index_grid
        SOURCES     index_grid.cc
		CONDITION   HAVE_GRID AND ATLAS_FOUND AND GRIB_API_FOUND
		INCLUDES    ${ATLAS_INCLUDE_DIRS} ${GRIB_API_INCLUDE_DIRS}
		LIBS        eckit ${ATLAS_LIBRARIES} ${GRIB_API_LIBRARIES}
	)

