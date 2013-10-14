list( APPEND eckit_grid_srcs
Field.h
Field.cc )


ecbuild_add_library( TARGET eckit_grid
					 INSTALL_HEADERS ALL
					 SOURCES
						${eckit_grid_srcs}
					 HEADER_DESTINATION
						include/eckit/grid
					 LIBS
						eckit )

