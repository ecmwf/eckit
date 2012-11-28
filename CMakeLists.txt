############################################################################################
# cmake options:
#
#       -DCMAKE_BUILD_TYPE=Debug|RelWithDebInfo|Release|Production
#       -DCMAKE_INSTALL_PREFIX=/path/to/install
#
#       -DCMAKE_MODULE_PATH=/path/to/ecbuild/cmake
#
#       -DCMAKE_C_COMPILER=gcc
#       -DCMAKE_C_COMPILER=g++
#
#       -DCMAKE_PREFIX_PATH=/path/to/jasper:/path/to/any/package/out/of/place
#       -DBUILD_SHARED_LIBS=OFF

cmake_minimum_required( VERSION 2.6.4 FATAL_ERROR )

project( eclib CXX )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

include( ecbuild_system )

############################################################################################
# go-no-go tests

# if we use off_t instead of real off64_t,
# then lets check it has size 8
if( NOT EC_HAVE_OFF64T AND EC_HAVE_OFFT )
    if( NOT EC_SIZEOF_OFF_T EQUAL 8 )
        message( FATAL_ERROR "off_t hasn't 64 bits [${EC_SIZEOF_OFF_T}] and off64_t is not available" )
    endif()
endif()

# print warning if off_t is 4 bytes
if( EC_SIZEOF_OFF_T EQUAL 4 AND EC_HAVE_OFF64T )
  message( STATUS "\n" )
  message( STATUS "********************************************" )
  message( STATUS "NOTE: off_t has 32 bits -- using off64_t" )
  message( STATUS "********************************************" )
  message( STATUS "\n" )
endif()

###############################################################################
# local project

ecbuild_declare_project()

###############################################################################
# some variables/options of this project

find_program( PSTACK_COMMAND "pstack" DOC "path to the pstack executable" )
mark_as_advanced(PSTACK_COMMAND)

find_program( CPPFILT_COMMAND "c++filt" DOC "path to the c++filt executable" )
mark_as_advanced(CPPFILT_COMMAND)

###############################################################################
# macro processing

###############################################################################
# contents

ecbuild_find_project_files()

if( NOT ${PROJECT_NAME} STREQUAL ${CMAKE_PROJECT_NAME} )
    set( ECLIB_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src PARENT_SCOPE )
    set( ECLIB_LIBRARIES      Ec   PARENT_SCOPE )
    set( ECLIB_FOUND          TRUE PARENT_SCOPE )
endif()

# source files
add_subdirectory( src )

############################################################################################
# finalize

ecbuild_install_project( NAME eclib )

ecbuild_print_summary()
