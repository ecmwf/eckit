list( APPEND eckit_geometry_srcs
EllipsoidOfRevolution.cc
EllipsoidOfRevolution.h
GreatCircle.cc
GreatCircle.h
KPoint.cc
KPoint.h
Point2.cc
Point2.h
Point3.cc
Point3.h
Polygon.cc
Polygon.h
Sphere.cc
Sphere.h
SphereT.h
UnitSphere.h
)

ecbuild_add_library(
					TARGET eckit_geometry
					TYPE SHARED
					INSTALL_HEADERS ALL
					HEADER_DESTINATION
						${INSTALL_INCLUDE_DIR}/eckit/geometry
					SOURCES
						${eckit_geometry_srcs}
					PUBLIC_LIBS
						eckit  )
