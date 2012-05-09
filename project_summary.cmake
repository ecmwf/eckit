if( SWIG_FOUND )
  message( STATUS " SWIG command     : [${SWIG_EXECUTABLE}]" )
endif()

if(ODB_FOUND)
  message( STATUS " ODB     include  : [${ODB_INCLUDE_DIRS}]" )
  message( STATUS "         libs     : [${ODB_LIBRARIES}]" )
  message( STATUS "     DL  lib      : [${DL_LIBRARIES}]" )
endif()

