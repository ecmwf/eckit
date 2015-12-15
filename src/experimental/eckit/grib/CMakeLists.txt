list( APPEND eckit_grib_srcs
EmosFile.cc
EmosFile.h
Grib.h
GribAccessor.cc
GribAccessor.h
GribField.cc
GribField.h
GribFieldMemoryStrategy.cc
GribFieldMemoryStrategy.h
GribFieldSet.cc
GribFieldSet.cc
GribFieldSet.h
GribFieldSet.h
GribFile.cc
GribFile.h
GribHandle.cc
GribHandle.h
GribMutator.cc
GribMutator.h
GribParams.cc
GribParams.h
GribReader.cc
GribReader.h
PointIndex.cc
PointIndex.h
)


ecbuild_add_library(TARGET eckit_grib
					INSTALL_HEADERS ALL
					CONDITION	HAVE_GRIB
					SOURCES ${eckit_grib_srcs}
					HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/grib
					INCLUDES ${GRIB_API_INCLUDE_DIRS}
					LIBS eckit_geometry eckit ${GRIB_API_LIBRARIES} )

