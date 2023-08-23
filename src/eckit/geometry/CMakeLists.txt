
configure_file( eckit_geometry_config.h.in eckit_geometry_config.h @ONLY )
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/eckit_geometry_config.h
    DESTINATION ${INSTALL_INCLUDE_DIR}/eckit
)

list( APPEND eckit_geometry_srcs
    ${CMAKE_CURRENT_BINARY_DIR}/eckit_geometry_config.h
    Area.h
    Cache.cc
    Cache.h
    Configurator.h
    CoordinateHelpers.cc
    CoordinateHelpers.h
    Domain.cc
    Domain.h
    Earth.h
    EllipsoidOfRevolution.cc
    EllipsoidOfRevolution.h
    Figure.h
    GreatCircle.cc
    GreatCircle.h
    Grid.cc
    Grid.h
    GridConfig.cc
    GridConfig.h
    Iterator.h
    KPoint.cc
    KPoint.h
    LibEcKitGeometry.cc
    LibEcKitGeometry.h
    Ordering.h
    Point.cc
    Point.h
    Point2.cc
    Point2.h
    Point3.cc
    Point3.h
    PointLonLat.cc
    PointLonLat.h
    Projection.h
    Range.cc
    Range.h
    Renumber.h
    Search.h
    Sphere.cc
    Sphere.h
    SphereT.h
    UnitSphere.h
    area/BoundingBox.cc
    area/BoundingBox.h
    grid/Gaussian.cc
    grid/Gaussian.h
    grid/GaussianIterator.cc
    grid/GaussianIterator.h
    grid/HEALPix.cc
    grid/HEALPix.h
    grid/IrregularLatlon.cc
    grid/IrregularLatlon.h
    grid/ORCA.cc
    grid/ORCA.h
    grid/ReducedGG.cc
    grid/ReducedGG.h
    grid/ReducedLL.cc
    grid/ReducedLL.h
    grid/Regular.cc
    grid/Regular.h
    grid/RegularGG.cc
    grid/RegularGG.h
    grid/RegularGrid.cc
    grid/RegularGrid.h
    grid/UnstructuredGrid.cc
    grid/UnstructuredGrid.h
    iterator/ListI.cc
    iterator/ListI.h
    iterator/ListIListJ.cc
    iterator/ListIListJ.h
    iterator/ReducedIListJ.cc
    iterator/ReducedIListJ.h
    polygon/LonLatPolygon.cc
    polygon/LonLatPolygon.h
    polygon/Polygon.cc
    polygon/Polygon.h
    projection/LonLatToXYZ.cc
    projection/LonLatToXYZ.h
    projection/None.cc
    projection/None.h
    projection/Rotation.cc
    projection/Rotation.h
    util.cc
    util.h
    util/arange.cc
    util/gaussian_latitudes.cc
    util/linspace.cc
    util/monotonic_crop.cc
    util/reduced_classical_pl.cc
    util/reduced_octahedral_pl.cc
    util/regex.cc
    util/regex.h
)

set(eckit_geometry_include_dirs
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>
)

set(eckit_geometry_libs eckit_maths eckit_codec)

if(HAVE_PROJ)
    list(APPEND eckit_geometry_srcs projection/PROJ.cc projection/PROJ.h)
    list(APPEND eckit_geometry_libs PROJ::proj)
    list(APPEND eckit_geometry_include_dirs ${PROJ_INCLUDE_DIRS})
endif()

ecbuild_add_library(
    TARGET eckit_geometry
    TYPE   SHARED

    INSTALL_HEADERS    ALL
    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/geometry
    PUBLIC_LIBS        ${eckit_geometry_libs}
    PUBLIC_INCLUDES    ${eckit_geometry_include_dirs}

    SOURCES ${eckit_geometry_srcs}
)

