
list(APPEND eckit_geo_srcs
    Area.cc
    Area.h
    Cache.cc
    Cache.h
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
    LibEcKitGeo.cc
    LibEcKitGeo.h
    Ordering.cc
    Ordering.h
    Point.cc
    Point.h
    Point2.cc
    Point2.h
    Point3.cc
    Point3.h
    PointLonLat.cc
    PointLonLat.h
    PointLonLatR.cc
    PointLonLatR.h
    Projection.cc
    Projection.h
    Range.cc
    Range.h
    Renumber.h
    Search.h
    Shape.cc
    Shape.h
    Spec.cc
    Spec.h
    area/BoundingBox.cc
    area/BoundingBox.h
    etc/Grid.cc
    etc/Grid.h
    figure/Earth.cc
    figure/Earth.h
    figure/OblateSpheroid.cc
    figure/OblateSpheroid.h
    figure/Sphere.cc
    figure/Sphere.h
    figure/UnitSphere.cc
    figure/UnitSphere.h
    geometry/OblateSpheroid.cc
    geometry/OblateSpheroid.h
    geometry/Sphere.cc
    geometry/Sphere.h
    geometry/SphereT.h
    geometry/UnitSphere.h
    grid/HEALPix.cc
    grid/HEALPix.h
    grid/Reduced.cc
    grid/Reduced.h
    grid/ReducedGaussian.cc
    grid/ReducedGaussian.h
    grid/ReducedLL.cc
    grid/ReducedLL.h
    grid/Regular.cc
    grid/Regular.h
    grid/RegularGaussian.cc
    grid/RegularGaussian.h
    grid/RegularLL.cc
    grid/RegularLL.h
    grid/RegularXY.cc
    grid/RegularXY.h
    grid/Unstructured.cc
    grid/Unstructured.h
    grid/regular-xy/LambertAzimuthalEqualArea.cc
    grid/regular-xy/LambertAzimuthalEqualArea.h
    grid/regular-xy/LambertConformalConic.cc
    grid/regular-xy/LambertConformalConic.h
    grid/regular-xy/Mercator.cc
    grid/regular-xy/Mercator.h
    grid/regular-xy/PolarStereographic.cc
    grid/regular-xy/PolarStereographic.h
    grid/regular-xy/SpaceView.cc
    grid/regular-xy/SpaceView.h
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
    projection/LambertAzimuthalEqualArea.cc
    projection/LambertAzimuthalEqualArea.h
    projection/LambertConformalConic.cc
    projection/LambertConformalConic.h
    projection/LonLatToXYZ.cc
    projection/LonLatToXYZ.h
    projection/Mercator.cc
    projection/Mercator.h
    projection/None.cc
    projection/None.h
    projection/PolarStereographic.cc
    projection/PolarStereographic.h
    projection/ProjectionOnFigure.cc
    projection/ProjectionOnFigure.h
    projection/Reverse.h
    projection/Rotation.cc
    projection/Rotation.h
    projection/SpaceView.cc
    projection/SpaceView.h
    projection/Stretch.cc
    projection/Stretch.h
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
    util/sincos.h
)

set(eckit_geo_include_dirs
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>
)

set(eckit_geo_libs eckit_maths)
set(eckit_GEO_ETC_GRID "~eckit/etc/eckit/geo/grid.yaml")

if(eckit_HAVE_PROJ)
    list(APPEND eckit_geo_srcs projection/PROJ.cc projection/PROJ.h)
    list(APPEND eckit_geo_libs PROJ::proj)
endif()

if(eckit_HAVE_GEO_GRID_ORCA)
    list(APPEND eckit_geo_srcs grid/ORCA.cc grid/ORCA.h)
    list(APPEND eckit_geo_libs eckit_codec)
    list(APPEND eckit_GEO_ETC_GRID "~eckit/etc/eckit/geo/ORCA.yaml")
endif()

string(REPLACE ";" ":" eckit_GEO_ETC_GRID "${eckit_GEO_ETC_GRID}")
configure_file(eckit_geo_config.h.in eckit_geo_config.h @ONLY)

list(APPEND eckit_geo_srcs ${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_config.h)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_config.h
    DESTINATION ${INSTALL_INCLUDE_DIR}/eckit
)

ecbuild_add_library(
    TARGET             eckit_geo
    TYPE               SHARED
    INSTALL_HEADERS    ALL
    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/geo
    PUBLIC_LIBS        ${eckit_geo_libs}
    PUBLIC_INCLUDES    ${eckit_geo_include_dirs}
    SOURCES            ${eckit_geo_srcs}
)
