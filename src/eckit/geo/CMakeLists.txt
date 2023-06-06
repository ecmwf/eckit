list(APPEND eckit_geo_srcs
    BoundingBox.cc
    BoundingBox.h
    Domain.cc
    Domain.h
    Grid.cc
    Grid.h
    Iterator.cc
    Iterator.h
    Point.cc
    Point.h
    Projection.cc
    Projection.h
    Range.cc
    Range.h
    Renumber.h
    Scanner.cc
    Scanner.h
    Search.h
    grid/Albers.cc
    grid/Albers.h
    grid/AzimuthRange.cc
    grid/AzimuthRange.h
    grid/Classic.cc
    grid/Classic.h
    grid/EquatorialAzimuthalEquidistant.cc
    grid/EquatorialAzimuthalEquidistant.h
    grid/FromPL.cc
    grid/FromPL.h
    grid/Gaussian.cc
    grid/Gaussian.h
    grid/GaussianIterator.cc
    grid/GaussianIterator.h
    grid/IrregularLatlon.cc
    grid/IrregularLatlon.h
    grid/Lambert.cc
    grid/Lambert.h
    grid/LambertAzimuthalEqualArea.cc
    grid/LambertAzimuthalEqualArea.h
    grid/LatLon.cc
    grid/LatLon.h
    grid/Mercator.cc
    grid/Mercator.h
    grid/None.cc
    grid/None.h
    grid/ORCA.cc
    grid/ORCA.h
    grid/Octahedral.cc
    grid/Octahedral.h
    grid/PolarStereographic.cc
    grid/PolarStereographic.h
    grid/Reduced.cc
    grid/Reduced.h
    grid/ReducedClassic.cc
    grid/ReducedClassic.h
    grid/ReducedFromPL.cc
    grid/ReducedFromPL.h
    grid/ReducedLL.cc
    grid/ReducedLL.h
    grid/ReducedOctahedral.cc
    grid/ReducedOctahedral.h
    grid/Regular.cc
    grid/Regular.h
    grid/RegularGG.cc
    grid/RegularGG.h
    grid/RegularGrid.cc
    grid/RegularGrid.h
    grid/RegularLL.cc
    grid/RegularLL.h
    grid/RotatedClassic.cc
    grid/RotatedClassic.h
    grid/RotatedFromPL.cc
    grid/RotatedFromPL.h
    grid/RotatedGG.cc
    grid/RotatedGG.h
    grid/RotatedLL.cc
    grid/RotatedLL.h
    grid/RotatedOctahedral.cc
    grid/RotatedOctahedral.h
    grid/SpaceView.cc
    grid/SpaceView.h
    grid/StretchedGG.cc
    grid/StretchedGG.h
    grid/StretchedLL.cc
    grid/StretchedLL.h
    grid/StretchedRotatedGG.cc
    grid/StretchedRotatedGG.h
    grid/StretchedRotatedLL.cc
    grid/StretchedRotatedLL.h
    grid/TransverseMercator.cc
    grid/TransverseMercator.h
    grid/UnstructuredGrid.cc
    grid/UnstructuredGrid.h
    iterator/IteratorAggregator.h
    iterator/IteratorComposer.cc
    iterator/IteratorComposer.h
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

set(eckit_geo_include_dirs )
set(eckit_geo_libs eckit_geometry eckit_maths)

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

ecbuild_add_library(
    TARGET eckit_geo_grib
    TYPE SHARED
    CONDITION HAVE_ECCODES
    SOURCES grib/GribConfiguration.cc grib/GribConfiguration.h
    PUBLIC_LIBS eckit_geo eccodes
)

