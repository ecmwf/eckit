ecbuild_add_option(
  FEATURE DOCS
  DESCRIPTION "Generate reference documentation"
  REQUIRED_PACKAGES "Doxygen" )

if( HAVE_DOCS )
  set( DOXYFILE Doxyfile CACHE INTERNAL "Doxygen filename" )
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${DOXYFILE}.in "${CMAKE_CURRENT_BINARY_DIR}/${DOXYFILE}" @ONLY )
  add_custom_target(doc
  COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/${DOXYFILE}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
endif()
