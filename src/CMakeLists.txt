# TODO: eventually this should be removed
#       as ifdef's should be based on features not for OS name
add_definitions( -D${EC_OS_NAME} )

### config headers

configure_file( eckit_config.h.in   eckit_config.h  )
configure_file( eckit_version.h.in  eckit_version.h )

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/eckit_config.h 
               ${CMAKE_CURRENT_BINARY_DIR}/eckit_version.h 
         DESTINATION ${INSTALL_INCLUDE_DIR} )

### include directories

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

### sources

add_subdirectory( eckit  )

add_subdirectory( apps )

add_subdirectory( tests )
