list( APPEND eckit_geometry_srcs
EllipsoidOfRevolution.cc
EllipsoidOfRevolution.h
GreatCircle.cc
GreatCircle.h
KPoint.h
Point2.cc
Point2.h
Point3.cc
Point3.h
Sphere.cc
Sphere.h
SphereT.h
UnitSphere.h
)

ecbuild_add_library(TARGET eckit_geometry
					INSTALL_HEADERS ALL
					HEADER_DESTINATION
						${INSTALL_INCLUDE_DIR}/eckit/geometry
					SOURCES
						${eckit_geometry_srcs}
					LIBS
						eckit  )

