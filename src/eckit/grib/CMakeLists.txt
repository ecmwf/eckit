list( APPEND eckit_grib_srcs
EmosFile.cc
EmosFile.h
Grib.h
GribAccessor.h
GribAccessor.cc
GribField.cc
GribField.h
GribFieldSet.cc
GribFieldSet.cc
GribFieldSet.h
GribFieldSet.h
GribFile.cc
GribFile.h
GribHandle.cc
GribHandle.h
GribFieldMemoryStrategy.h
GribFieldMemoryStrategy.cc
PointIndex.cc
PointIndex.h
GribReader.cc
GribReader.h
)


ecbuild_add_library(TARGET eckit_grib
					INSTALL_HEADERS ALL
					CONDITION	HAVE_GRIB
					SOURCES ${eckit_grib_srcs}
					HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/grib
					INCLUDES ${GRIB_API_INCLUDE_DIRS}
					LIBS eckit ${GRIB_API_LIBRARIES} )

