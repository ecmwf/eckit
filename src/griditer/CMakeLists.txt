configure_file(griditer.h.in griditer.h)

add_library(grit SHARED
    util.h
    util/arange.cc
    util/linspace.cc)

target_include_directories(grit PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

