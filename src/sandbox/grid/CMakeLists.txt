list( APPEND eckit_tesselation_srcs
PointIndex3.h
PointIndex3.cc
GribRead.h
GribRead.cc
GribWrite.h
GribWrite.cc
TriangleIntersection.h
PointSet.h
PointSet.cc
FloatCompare.h
FloatCompare.cc
Tesselation.h
Tesselation.cc )

ecbuild_add_library(
		TARGET      eckit_tesselation
		SOURCES     ${eckit_tesselation_srcs}
		CONDITION   HAVE_TESSELATION AND HAVE_GRID AND ATLAS_FOUND AND GRIB_API_FOUND AND HAVE_EIGEN AND Boost_FOUND
		INCLUDES    ${ATLAS_INCLUDE_DIRS} ${GRIB_API_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR} ${CGAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
		LIBS        eckit ${ATLAS_LIBRARIES} ${GRIB_API_LIBRARIES} ${CGAL_LIBRARY} ${CGAL_3RD_PARTY_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} )


ecbuild_add_test(
		TARGET      interpolate
		SOURCES     interpolate.cc
		CONDITION   HAVE_GRID AND ATLAS_FOUND AND GRIB_API_FOUND AND HAVE_EIGEN AND Boost_FOUND
		INCLUDES    ${ATLAS_INCLUDE_DIRS} ${GRIB_API_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR} ${CGAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
		LIBS        eckit_tesselation )

