# TODO: eventually this should be removed
#       as ifdef's should be based on features not for OS name
add_definitions( -D${EC_OS_NAME} )

### config headers

configure_file( eclib_config.h.in   eclib_config.h  )
configure_file( eclib_version.h.in  eclib_version.h )

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/eclib_config.h 
               ${CMAKE_CURRENT_BINARY_DIR}/eclib_version.h 
         DESTINATION include/${PROJECT_NAME} )

### include directories

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

### sources

add_subdirectory( eclib  )

add_subdirectory( apps )

add_subdirectory( tests )
