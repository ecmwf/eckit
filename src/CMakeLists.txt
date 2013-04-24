### config headers

configure_file( eckit_config.h.in   eckit_config.h  )
configure_file( eckit_version.h.in  eckit_version.h )

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/eckit_config.h 
               ${CMAKE_CURRENT_BINARY_DIR}/eckit_version.h 
         DESTINATION ${INSTALL_INCLUDE_DIR} )

### sources

add_subdirectory( eckit  )

add_subdirectory( apps )

add_subdirectory( tests )
