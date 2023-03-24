configure_file(grit.h.in grit.h)

add_library(grit SHARED
    util.cc
    util.h
    util/arange.cc
    util/gaussian_latitudes.cc
    util/linspace.cc)

target_include_directories(grit PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

