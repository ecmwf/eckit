list( APPEND eckit_grib_srcs
GribField.cc
GribField.h
GribFieldSet.cc
GribFieldSet.h
GribFile.cc
GribFile.h
EmosFile.cc
EmosFile.h
GribHandle.cc
GribHandle.h
GribFieldSet.cc
GribFieldSet.h
GribFieldState.cc
GribFieldState.h
GribFieldStateFile.cc
GribFieldStateFile.h
GribFieldStateExpanded.cc
GribFieldStateExpanded.h
GribCompute.cc
GribCompute.h
GribFieldStateHandle.cc
GribFieldStateHandle.h
GribFieldStateResult.cc
GribFieldStateResult.h
 )


ecbuild_add_library(TARGET eckit_grib
					INSTALL_HEADERS ALL
					CONDITION	ECKIT_GRIB
					SOURCES ${eckit_grib_srcs}
					HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/grib
					INCLUDES ${GRIB_API_INCLUDE_DIRS}
					LIBS eckit ${GRIB_API_LIBRARIES})

