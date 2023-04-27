list(APPEND TOOLS
    grit-version)

if(eckit_FOUND)
    list(APPEND TOOLS grit-search)
endif()

foreach(tool ${TOOLS})
    add_executable(${tool} ${tool}.cc)
    target_link_libraries(${tool} grit)
endforeach()

if(TARGET eccodes)
    add_executable(grit-grib grit-grib.cc)
    target_link_libraries(grit-grib grit eccodes)
endif()

