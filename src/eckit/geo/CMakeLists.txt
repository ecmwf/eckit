
configure_file(eckit_geo_config.h.in eckit_geo_config.h @ONLY)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_config.h
    DESTINATION ${INSTALL_INCLUDE_DIR}/eckit
)

list(APPEND eckit_geo_srcs
    ${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_config.h
    Area.cc
    Area.h
    Cache.cc
    Cache.h
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
    Iterator.h
    LibEcKitGeo.cc
    LibEcKitGeo.h
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
    Spec.cc
    Spec.h
    Sphere.cc
    Sphere.h
    SphereT.h
    UnitSphere.h
    area/BoundingBox.cc
    area/BoundingBox.h
    etc/Grid.cc
    etc/Grid.h
    figure/OblateSpheroid.cc
    figure/OblateSpheroid.h
    figure/Sphere.cc
    figure/Sphere.h
    grid/ReducedGlobal.cc
    grid/ReducedGlobal.h
    grid/ReducedLocal.cc
    grid/ReducedLocal.h
    grid/Regular.cc
    grid/Regular.h
    grid/Unstructured.cc
    grid/Unstructured.h
    grid/reduced-global/HEALPix.cc
    grid/reduced-global/HEALPix.h
    grid/reduced-global/ReducedGaussian.cc
    grid/reduced-global/ReducedGaussian.h
    grid/reduced-local/ReducedLL.cc
    grid/reduced-local/ReducedLL.h
    grid/regular/IrregularLL.cc
    grid/regular/IrregularLL.h
    grid/regular/RegularGaussian.cc
    grid/regular/RegularGaussian.h
    grid/regular/RegularLL.cc
    grid/regular/RegularLL.h
    grid/regular/XYToLonLat.cc
    grid/regular/XYToLonLat.h
    grid/unstructured/ORCA.cc
    grid/unstructured/ORCA.h
    grid/unstructured/UnstructuredFromGrid.cc
    grid/unstructured/UnstructuredFromGrid.h
    iterator/Reduced.cc
    iterator/Reduced.h
    iterator/Regular.cc
    iterator/Regular.h
    iterator/Unstructured.cc
    iterator/Unstructured.h
    polygon/LonLatPolygon.cc
    polygon/LonLatPolygon.h
    polygon/Polygon.cc
    polygon/Polygon.h
    projection/Composer.cc
    projection/Composer.h
    projection/LonLatToXY.cc
    projection/LonLatToXY.h
    projection/LonLatToXYZ.cc
    projection/LonLatToXYZ.h
    projection/Mercator.cc
    projection/Mercator.h
    projection/None.cc
    projection/None.h
    projection/Rotation.cc
    projection/Rotation.h
    projection/XYToLonLat.cc
    projection/XYToLonLat.h
    range/GaussianLatitude.cc
    range/GaussianLatitude.h
    range/Regular.cc
    range/Regular.h
    range/RegularCartesian.cc
    range/RegularCartesian.h
    range/RegularLatitude.cc
    range/RegularLatitude.h
    range/RegularLongitude.cc
    range/RegularLongitude.h
    spec/Custom.cc
    spec/Custom.h
    spec/Generator.h
    spec/Layered.cc
    spec/Layered.h
    util.cc
    util.h
    util/arange.cc
    util/bounding_box.cc
    util/gaussian_latitudes.cc
    util/linspace.cc
    util/monotonic_crop.cc
    util/mutex.h
    util/reduced_classical_pl.cc
    util/reduced_octahedral_pl.cc
    util/regex.cc
    util/regex.h
)

set(eckit_geo_include_dirs
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>
)

set(eckit_geo_libs eckit_maths)

if(eckit_HAVE_PROJ)
    list(APPEND eckit_geo_srcs projection/PROJ.cc projection/PROJ.h)
    list(APPEND eckit_geo_libs PROJ::proj)
endif()

if(eckit_HAVE_ECKIT_CODEC)
    list(APPEND eckit_geo_srcs grid/unstructured/ORCA.cc grid/unstructured/ORCA.h)
    list(APPEND eckit_geo_libs eckit_codec)
endif()

ecbuild_add_library(
    TARGET             eckit_geo
    TYPE               SHARED
    INSTALL_HEADERS    ALL
    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/geo
    PUBLIC_LIBS        ${eckit_geo_libs}
    PUBLIC_INCLUDES    ${eckit_geo_include_dirs}
    SOURCES            ${eckit_geo_srcs}
)
