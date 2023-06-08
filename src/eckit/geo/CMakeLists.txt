list(APPEND eckit_geo_srcs
    BoundingBox.cc
    BoundingBox.h
    Domain.cc
    Domain.h
    Earth.h
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
    Increments.cc
    Increments.h
    Scanner.cc
    Scanner.h
    Search.h
    grid/Gaussian.cc
    grid/Gaussian.h
    grid/GaussianIterator.cc
    grid/GaussianIterator.h
    grid/IrregularLatlon.cc
    grid/IrregularLatlon.h
    grid/ORCA.cc
    grid/ORCA.h
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
    grid/SpaceView.cc
    grid/SpaceView.h
    grid/UnstructuredGrid.cc
    grid/UnstructuredGrid.h
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

