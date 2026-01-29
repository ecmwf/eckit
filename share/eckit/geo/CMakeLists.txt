set(_files)

list(APPEND _files "area.yaml" "grid.yaml")

if(eckit_HAVE_GEO_AREA_SHAPEFILE)
    list(APPEND _files "shapefile.yaml")
endif()

if(ECKIT_GEO_CODEC_GRIDS)
    list(APPEND _files "FESOM.yaml" "ICON-CH.yaml" "ICON-DWD.yaml" "ICON-MPIM.yaml" "ORCA.yaml")
endif()

set(_destination "share/eckit/geo")

install(FILES ${_files} DESTINATION ${_destination} PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)

foreach(_file ${_files})
    configure_file(${_file} "${CMAKE_BINARY_DIR}/${_destination}/${_file}" COPYONLY)
endforeach()
