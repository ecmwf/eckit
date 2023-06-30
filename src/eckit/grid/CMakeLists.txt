list(APPEND eckit_grid_srcs
    BoundingBox.cc
    BoundingBox.h
    Domain.cc
    Domain.h
    Earth.h
    Figure.cc
    Figure.h
    Grid.cc
    Grid.h
    Increments.cc
    Increments.h
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

set(eckit_grid_include_dirs )
set(eckit_grid_libs eckit_geometry eckit_maths)

if(HAVE_PROJ)
    list(APPEND eckit_grid_srcs projection/PROJ.cc projection/PROJ.h)
    list(APPEND eckit_grid_libs PROJ::proj)
    list(APPEND eckit_grid_include_dirs ${PROJ_INCLUDE_DIRS})
endif()

ecbuild_add_library(
    TARGET eckit_grid
    TYPE SHARED
    INSTALL_HEADERS ALL
    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/grid
    SOURCES ${eckit_grid_srcs}
    PUBLIC_LIBS ${eckit_grid_libs}
    PUBLIC_INCLUDES ${eckit_grid_include_dirs}
)

ecbuild_add_library(
    TARGET eckit_grid_grib
    TYPE SHARED
    CONDITION HAVE_ECCODES
    SOURCES grib/GribConfiguration.cc grib/GribConfiguration.h
    PUBLIC_LIBS eckit_grid eccodes
)

