list( APPEND eckit_grib_srcs
EmosFile.cc
EmosFile.h
GribCompute.cc
GribCompute.h
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
Matrix.h 
Matrix.cc
)


ecbuild_add_library(TARGET eckit_grib
					INSTALL_HEADERS ALL
					CONDITION	ECKIT_GRIB
					SOURCES ${eckit_grib_srcs}
					HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/grib
					INCLUDES ${GRIB_API_INCLUDE_DIRS}
					LIBS eckit ${GRIB_API_LIBRARIES})

