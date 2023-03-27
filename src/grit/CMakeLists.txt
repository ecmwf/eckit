configure_file(grit.h.in grit.h)

add_library(grit SHARED
    Scanner.cc
    Scanner.h
    Transformation.cc
    Transformation.h
    util.cc
    util.h
    util/arange.cc
    util/gaussian_latitudes.cc
    util/linspace.cc)

target_include_directories(grit PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

if(TARGET PROJ::proj)
    target_link_libraries(grit PROJ::proj)
endif()

