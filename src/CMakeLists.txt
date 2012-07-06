include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

configure_file( eclib_config.h.in   eclib_config.h  )
configure_file( eclib_version.h.in  eclib_version.h )

install( FILES eclib_config.h eclib_version.h DESTINATION include/${PROJECT_NAME} )

add_subdirectory( eclib  )

add_subdirectory( tests )
