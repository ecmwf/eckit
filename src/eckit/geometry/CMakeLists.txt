list( APPEND eckit_geometry_srcs
    BoundingBox.cc
    BoundingBox.h
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
    detail/Gaussian.cc
    detail/Gaussian.h
    detail/GaussianIterator.cc
    detail/GaussianIterator.h
    detail/IrregularLatlon.cc
    detail/IrregularLatlon.h
    detail/ReducedGG.cc
    detail/ReducedGG.h
    detail/ReducedLL.cc
    detail/ReducedLL.h
    detail/RegularGG.cc
    detail/RegularGG.h
    detail/RegularGrid.cc
    detail/RegularGrid.h
    detail/RegularLL.cc
    detail/RegularLL.h
    detail/UnstructuredGrid.cc
    detail/UnstructuredGrid.h
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
    util/regular_pl.cc
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

