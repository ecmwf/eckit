list( APPEND eckit_geometry_srcs
Domain.h
Domain.cc
Point2.h
Point2.cc
Point3.h
Point3.cc
RotateGrid.h
RotateGrid.cc
)

ecbuild_add_library(TARGET eckit_geometry
					INSTALL_HEADERS ALL
					HEADER_DESTINATION
						${INSTALL_INCLUDE_DIR}/eckit/geometry
					SOURCES
						${eckit_geometry_srcs}
					LIBS
						eckit  )

