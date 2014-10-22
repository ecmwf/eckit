### Eigen

if( HAVE_EIGEN AND ECKIT_CONTRIB_EIGEN )

	install( DIRECTORY eigen3 DESTINATION ${INSTALL_INCLUDE_DIR} )

	### pack all contrib directory

	file( GLOB_RECURSE contrib_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*" )

	ecbuild_add_resources(  TARGET ${PROJECT_NAME}_contribs
							SOURCES_PACK ${contrib_files} )
endif()

