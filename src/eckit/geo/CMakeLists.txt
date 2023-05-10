list(APPEND eckit_geo_srcs
	Iterator.h
	Parametrisation.cc
	Parametrisation.h
	Projection.cc
	Projection.h
	Scanner.cc
	Scanner.h
	Search.h
	geometry/BoundingBox.cc
	geometry/BoundingBox.h
	geometry/GreatCircle.cc
	geometry/GreatCircle.h
	geometry/Sphere.cc
	geometry/Sphere.h
	geometry/Spheroid.cc
	geometry/Spheroid.h
	geometry/UnitSphere.h
	iterator/IteratorAggregator.h
	iterator/IteratorComposer.cc
	iterator/IteratorComposer.h
	param/Map.cc
	param/Map.h
	projection/LatLonToXYZ.cc
	projection/LatLonToXYZ.h
	projection/None.cc
	projection/None.h
	projection/Rotation.cc
	projection/Rotation.h
	scanner/Reduced.cc
	scanner/Reduced.h
	scanner/Regular.cc
	scanner/Regular.h
	scanner/Unstructured.cc
	scanner/Unstructured.h
	types.cc
	types.h
	types/MatrixXYZ.h
	types/PointLatLon.h
	types/PointXY.h
	types/PointXYZ.h
	util.cc
	util.h
	util/arange.cc
	util/gaussian_latitudes.cc
	util/linspace.cc
	util/monotonic_crop.cc
	util/reduced_classical_pl.cc
	util/reduced_octahedral_pl.cc
	util/regular_pl.cc
)

set(eckit_geo_include_dirs )
set(eckit_geo_libs eckit)

if(HAVE_PROJ)
	list(APPEND eckit_geo_srcs projection/PROJ.cc projection/PROJ.h)
	list(APPEND eckit_geo_libs PROJ::proj)
	list(APPEND eckit_geo_include_dirs ${PROJ_INCLUDE_DIRS})
endif()

ecbuild_add_library(
	TARGET eckit_geo
	TYPE SHARED
	INSTALL_HEADERS ALL
	HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/geo
	SOURCES ${eckit_geo_srcs}
	PUBLIC_LIBS ${eckit_geo_libs}
	PUBLIC_INCLUDES ${eckit_geo_include_dirs}
)

