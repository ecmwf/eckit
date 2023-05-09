configure_file(grit.h.in grit.h)

add_library(grit SHARED
	Parametrisation.cc
	Parametrisation.h
	iterator/IteratorAggregator.h
	iterator/IteratorComposer.cc
	iterator/IteratorComposer.h
    Iterator.h
    Projection.cc
    Projection.h
    Scanner.cc
    Scanner.h
    exception.h
    geometry/BoundingBox.cc
    geometry/BoundingBox.h
    geometry/GreatCircle.cc
    geometry/GreatCircle.h
    geometry/Sphere.cc
    geometry/Sphere.h
    geometry/Spheroid.cc
    geometry/Spheroid.h
    geometry/UnitSphere.h
    log.cc
    log.h
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
    test.cc
    test.h
    types.cc
    types.h
    types/MatrixXYZ.h
    types/PointLatLon.h
    types/PointXY.h
    types/PointXYZ.h
    util.cc
    util.h
    util/Mutex.h
    util/arange.cc
    util/gaussian_latitudes.cc
    util/linspace.cc
    util/monotonic_crop.cc
    util/reduced_classical_pl.cc
    util/reduced_octahedral_pl.cc
    util/regular_pl.cc)

target_include_directories(grit PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

if(eckit_FOUND)
    target_sources(grit PRIVATE Search.h)
    target_link_libraries(grit eckit)
endif()

if(TARGET PROJ::proj)
    target_sources(grit PRIVATE projection/PROJ.cc projection/PROJ.h)
    target_link_libraries(grit PROJ::proj)
endif()

