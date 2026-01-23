
list(APPEND eckit_geo_srcs
    Area.cc
    Area.h
    Arrangement.h
    Container.h
    Exceptions.cc
    Exceptions.h
    Figure.cc
    Figure.h
    GreatCircle.cc
    GreatCircle.h
    Grid.cc
    Grid.h
    Iterator.cc
    Iterator.h
    LibEcKitGeo.cc
    LibEcKitGeo.h
    Point.cc
    Point.h
    PointLonLat.cc
    PointLonLat.h
    PointLonLatR.cc
    PointLonLatR.h
    PointXY.cc
    PointXY.h
    PointXYZ.cc
    PointXYZ.h
    Projection.cc
    Projection.h
    Range.cc
    Range.h
    Search.cc
    Search.h
    Shape.cc
    Shape.h
    Trace.cc
    Trace.h
    area/BoundingBox.cc
    area/BoundingBox.h
    area/BoundingBoxXY.cc
    area/BoundingBoxXY.h
    area/Library.cc
    area/Library.h
    area/None.cc
    area/None.h
    area/Polygon.cc
    area/Polygon.h
    area/library/GeoJSON.cc
    area/library/GeoJSON.h
    cache/DiskCache.cc
    cache/DiskCache.h
    cache/Download.cc
    cache/Download.h
    cache/MemoryCache.cc
    cache/MemoryCache.h
    container/PointsContainer.cc
    container/PointsContainer.h
    figure/Earth.cc
    figure/Earth.h
    figure/OblateSpheroid.cc
    figure/OblateSpheroid.h
    figure/OblateSpheroidT.h
    figure/Sphere.cc
    figure/Sphere.h
    figure/SphereT.h
    figure/UnitSphere.cc
    figure/UnitSphere.h
    grid/Reduced.cc
    grid/Reduced.h
    grid/Regular.cc
    grid/Regular.h
    grid/SphericalHarmonics.cc
    grid/SphericalHarmonics.h
    grid/Unstructured.cc
    grid/Unstructured.h
    grid/reduced/HEALPix.cc
    grid/reduced/HEALPix.h
    grid/reduced/ReducedGaussian.cc
    grid/reduced/ReducedGaussian.h
    grid/reduced/ReducedLonLat.cc
    grid/reduced/ReducedLonLat.h
    grid/regular/RegularGaussian.cc
    grid/regular/RegularGaussian.h
    grid/regular/RegularLL.cc
    grid/regular/RegularLL.h
    grid/regular/RegularXY.cc
    grid/regular/RegularXY.h
    grid/unstructured/UnstructuredGeneric.cc
    grid/unstructured/UnstructuredGeneric.h
    iterator/Reduced.cc
    iterator/Reduced.h
    iterator/Regular.cc
    iterator/Regular.h
    iterator/Unstructured.cc
    iterator/Unstructured.h
    order/HEALPix.cc
    order/HEALPix.h
    order/Scan.cc
    order/Scan.h
    polygon/Polygon.cc
    polygon/Polygon.h
    polygon/PolygonXY.cc
    polygon/PolygonXY.h
    projection/AlbersEqualArea.cc
    projection/AlbersEqualArea.h
    projection/Composer.cc
    projection/Composer.h
    projection/EquidistantCylindrical.cc
    projection/EquidistantCylindrical.h
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
    projection/Reverse.h
    projection/Rotation.cc
    projection/Rotation.h
    projection/SpaceView.cc
    projection/SpaceView.h
    projection/Stretch.cc
    projection/Stretch.h
    range/GaussianLatitude.cc
    range/GaussianLatitude.h
    range/Regular.cc
    range/Regular.h
    search/Tree.cc
    search/Tree.h
    search/TreeMapped.cc
    search/TreeMapped.h
    search/TreeMappedAnonymousMemory.cc
    search/TreeMappedAnonymousMemory.h
    search/TreeMappedFile.cc
    search/TreeMappedFile.h
    search/TreeMemory.cc
    search/TreeMemory.h
    share/Area.cc
    share/Area.h
    share/Grid.cc
    share/Grid.h
    share/Projection.cc
    share/Projection.h
    util.cc
    util.h
    util/arange.cc
    util/gaussian_latitudes.cc
    util/gaussian_quadrature_weights.cc
    util/hash_vector.cc
    util/linspace.cc
    util/monotonic_crop.cc
    util/mutex.h
    util/reduced_classical_pl.cc
    util/reduced_octahedral_pl.cc
    util/reverse.cc
    util/sincos.h
)

set(eckit_geo_include_dirs
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>
)

set(eckit_geo_libs eckit_maths eckit_spec)
set(eckit_GEO_SHARE_AREA "~eckit/share/eckit/geo/area.yaml")
set(eckit_GEO_SHARE_GRID "~eckit/share/eckit/geo/grid.yaml")
set(eckit_GEO_SHARE_PROJECTION "~eckit/share/eckit/geo/projection.yaml")

if(eckit_HAVE_PROJ)
    list(APPEND eckit_geo_srcs projection/PROJ.cc projection/PROJ.h)
    list(APPEND eckit_geo_libs PROJ::proj)
endif()

if(ECKIT_GEO_CODEC_GRIDS)
    list(APPEND eckit_geo_srcs
        grid/unstructured/FESOM.cc
        grid/unstructured/FESOM.h
        grid/unstructured/ICON.cc
        grid/unstructured/ICON.h
        grid/ORCA.cc grid/ORCA.h
    )

    list(APPEND eckit_GEO_SHARE_GRID
        "~eckit/share/eckit/geo/FESOM.yaml"
        "~eckit/share/eckit/geo/ICON-CH.yaml"
        "~eckit/share/eckit/geo/ICON-DWD.yaml"
        "~eckit/share/eckit/geo/ICON-MPIM.yaml"
        "~eckit/share/eckit/geo/ORCA.yaml"
    )

    list(APPEND eckit_geo_libs eckit_codec)
endif()

if(eckit_HAVE_GEO_AREA_SHAPEFILE)
    list(APPEND eckit_geo_srcs
        area/library/Shapefile.cc
        area/library/Shapefile.h
        cache/Unzip.cc
        cache/Unzip.h
    )
    list(APPEND eckit_geo_libs shapelib::shp libzip::zip)
endif()

string(REPLACE ";" ":" eckit_GEO_SHARE_GRID "${eckit_GEO_SHARE_GRID}")
string(REPLACE ";" ":" eckit_GEO_SHARE_AREA "${eckit_GEO_SHARE_AREA}")
string(REPLACE ";" ":" eckit_GEO_SHARE_PROJECTION "${eckit_GEO_SHARE_PROJECTION}")

configure_file(eckit_geo_config.h.in eckit_geo_config.h @ONLY)

list(APPEND eckit_geo_srcs ${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_config.h)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/eckit_geo_config.h
    DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/geo
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
