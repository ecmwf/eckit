ecbuild_add_option(
  FEATURE DOCS
  DESCRIPTION "Generate reference documentation"
  REQUIRED_PACKAGES "Doxygen" )

if( HAVE_DOCS )

  set( ECKIT_DOC eckit-doc )
  if( PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME )
    set( ECKIT_DOC doc )
  endif()

  set( DOXYFILE Doxyfile CACHE INTERNAL "Doxygen filename" )
  add_custom_target( ${ECKIT_DOC}
  COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/${DOXYFILE}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
endif()
