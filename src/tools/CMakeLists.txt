
foreach(name
        grit-version)
    add_executable(${name} ${name}.cc)
    target_link_libraries(${name} grit)
endforeach()

if(TARGET eccodes)
    add_executable(grit-grib grit-grib.cc)
    target_link_libraries(grit-grib grit eccodes)
endif()

