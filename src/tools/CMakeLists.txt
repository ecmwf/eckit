
foreach(name
        griditer-version)
    add_executable(${name} ${name}.cc)
    target_link_libraries(${name} grit)
endforeach()

if(TARGET eccodes)
    add_executable(griditer-grib griditer-grib.cc)
    target_link_libraries(griditer-grib grit eccodes)
endif()

