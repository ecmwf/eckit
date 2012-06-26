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

project( eclib )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

include( ecbuild_system )

###############################################################################
# local project

ecbuild_declare_project()

###############################################################################
# some variables/options of this project

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
