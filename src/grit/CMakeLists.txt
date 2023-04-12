configure_file(grit.h.in grit.h)

add_library(grit SHARED
    Figure.cc
    Figure.h
    Iterator.cc
    Iterator.h
    Projection.cc
    Projection.h
    Scanner.cc
    Scanner.h
    Transformation.cc
    Transformation.h
    exception.h
    figure/Sphere.cc
    figure/Sphere.h
    figure/Spheroid.cc
    figure/Spheroid.h
    geometry/BoundingBox.cc
    geometry/BoundingBox.h
    geometry/GreatCircle.cc
    geometry/GreatCircle.h
    geometry/Sphere.cc
    geometry/Sphere.h
    geometry/Spheroid.cc
    geometry/Spheroid.h
    geometry/UnitSphere.h
    types.cc
    types.h
    util.cc
    util.h
    util/arange.cc
    util/gaussian_latitudes.cc
    util/linspace.cc
    util/monotonic_crop.cc
    util/reduced_classical_pl.cc
    util/reduced_octahedral_pl.cc
    util/regular_pl.cc)

target_include_directories(grit PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

if(TARGET PROJ::proj)
    target_link_libraries(grit PROJ::proj)
endif()

