configure_file(griditer.h.in griditer.h)

add_library(grit SHARED
    util.h
    util/arange.cc
    util/linspace.cc)

