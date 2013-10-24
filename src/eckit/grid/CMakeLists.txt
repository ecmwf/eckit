list( APPEND eckit_grid_srcs
Field.cc
Field.h
Grid.cc
Grid.h
LatLon.cc
LatLon.h
 )


ecbuild_add_library( TARGET eckit_grid
					 INSTALL_HEADERS ALL
					 SOURCES
						${eckit_grid_srcs}
					 HEADER_DESTINATION
						${INSTALL_INCLUDE_DIR}/eckit/grid
					 LIBS
						eckit )

