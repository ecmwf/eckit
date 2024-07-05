set(_files)
list(APPEND _files "grid.yaml")
list(APPEND _files "ORCA.yaml")

set(_destination "etc/eckit/geo")

install(FILES ${_files} DESTINATION ${_destination} PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)

foreach(_file ${_files})
    configure_file(${_file} "${CMAKE_BINARY_DIR}/${_destination}/${_file}" COPYONLY)
endforeach()

