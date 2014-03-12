list( APPEND local_grid_headers PointIndex3.h TriangleIntersection.h)

ecbuild_add_test(
        TARGET      index_grid
		SOURCES     index_grid.cc ${local_grid_headers}
		CONDITION   HAVE_GRID AND ATLAS_FOUND AND GRIB_API_FOUND AND HAVE_EIGEN
		INCLUDES    ${ATLAS_INCLUDE_DIRS} ${GRIB_API_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR}
		LIBS        eckit ${ATLAS_LIBRARIES} ${GRIB_API_LIBRARIES}
	)

ecbuild_add_test(
        TARGET      index_points
		SOURCES     index_points.cc ${local_grid_headers}
		CONDITION   HAVE_GRID AND ATLAS_FOUND AND GRIB_API_FOUND AND HAVE_EIGEN
		INCLUDES    ${ATLAS_INCLUDE_DIRS} ${GRIB_API_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR}
		LIBS        eckit ${ATLAS_LIBRARIES} ${GRIB_API_LIBRARIES}
	)

