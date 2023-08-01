list( APPEND eckit_geometry_srcs
    Area.cc
    Area.h
    CoordinateHelpers.cc
    CoordinateHelpers.h
    Domain.cc
    Domain.h
    Earth.h
    EllipsoidOfRevolution.cc
    EllipsoidOfRevolution.h
    Figure.cc
    Figure.h
    GreatCircle.cc
    GreatCircle.h
    Grid.cc
    Grid.h
    Increments.cc
    Increments.h
    Iterator.cc
    Iterator.h
    KPoint.cc
    KPoint.h
    Point.cc
    Point.h
    Point2.cc
    Point2.h
    Point3.cc
    Point3.h
    PointLonLat.cc
    PointLonLat.h
    Projection.cc
    Projection.h
    Range.cc
    Range.h
    Renumber.h
    Scanner.cc
    Scanner.h
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
    grid/ReducedGG.cc
    grid/ReducedGG.h
    grid/ReducedLL.cc
    grid/ReducedLL.h
    grid/RegularGG.cc
    grid/RegularGG.h
    grid/RegularGrid.cc
    grid/RegularGrid.h
    grid/RegularLL.cc
    grid/RegularLL.h
    grid/UnstructuredGrid.cc
    grid/UnstructuredGrid.h
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
    scanner/Reduced.cc
    scanner/Reduced.h
    scanner/Regular.cc
    scanner/Regular.h
    scanner/Unstructured.cc
    scanner/Unstructured.h
    util.cc
    util.h
    util/arange.cc
    util/gaussian_latitudes.cc
    util/linspace.cc
    util/monotonic_crop.cc
    util/reduced_classical_pl.cc
    util/reduced_octahedral_pl.cc
)

set(eckit_geometry_include_dirs )
set(eckit_geometry_libs eckit_maths)

if(HAVE_PROJ)
    list(APPEND eckit_geometry_srcs projection/PROJ.cc projection/PROJ.h)
    list(APPEND eckit_geometry_libs PROJ::proj)
    list(APPEND eckit_geometry_include_dirs ${PROJ_INCLUDE_DIRS})
endif()

ecbuild_add_library(
    TARGET eckit_geometry
    TYPE SHARED
    INSTALL_HEADERS ALL
    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/geometry
    SOURCES ${eckit_geometry_srcs}
    PUBLIC_LIBS ${eckit_geometry_libs}
    PUBLIC_INCLUDES ${eckit_geometry_include_dirs}
)

