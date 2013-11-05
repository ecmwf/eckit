list( APPEND eckit_grib_srcs
GribField.cc
GribField.h
GribFieldSet.cc
GribFieldSet.h
GribFile.cc
GribFile.h
EmosFile.cc
EmosFile.h
 )


ecbuild_add_library(TARGET eckit_grib
					INSTALL_HEADERS ALL
					CONDITION	ECKIT_GRIB
					SOURCES ${eckit_grib_srcs}
					HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/grib
					LIBS eckit )

