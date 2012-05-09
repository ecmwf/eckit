###############################################################################
# cmake
#   options:
#       -DCMAKE_BUILD_TYPE=Debug|RelWithDebInfo|Release|Production
#       -DCMAKE_INSTALL_PREFIX=/path/to/install
#
#       -DCMAKE_C_COMPILER=gcc
#       -DCMAKE_C_COMPILER=g++
#
#       -DCMAKE_PREFIX_PATH=/path/to/jasper:/path/to/any/package/out/of/place

cmake_minimum_required( VERSION 2.6.4 FATAL_ERROR )

project( eclib C CXX )

###############################################################################
# include our cmake macros, but only do so if this is the top project
if( ${PROJECT_NAME} STREQUAL ${CMAKE_PROJECT_NAME} )

    set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

    # add backward support from 2.8 to 2.6
    if( ${CMAKE_VERSION} VERSION_LESS "2.8" )
        set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/2.8" ${CMAKE_MODULE_PATH} )
    endif()

    include(CTest)                 # add cmake testing support
    enable_testing()

    include(ecmwf_include_macros)  # define our macros
    include(ecmwf_define_options)  # define build options
    include(ecmwf_check_os)        # check for os characteristics
    include(ecmwf_check_functions) # check for available functions
    include(ecmwf_find_packages)   # find packages we depend on

endif()

###############################################################################
# local project

ecmwf_declare_project()

###############################################################################
# some variables/options of this project

###############################################################################
# macro processing

# define installation paths
include(ecmwf_define_paths)


###############################################################################
# contents

set( ECLIB_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src PARENT_SCOPE )
set( ECLIB_LIBRARIES      Ec   PARENT_SCOPE )
set( ECLIB_FOUND          TRUE PARENT_SCOPE )

# source files
add_subdirectory( src )

############################################################################################
# installation

ecmwf_install_project( NAME eclib )

###############################################################################
# summary

# print the summary of the configuration
include( ecmwf_print_summary )

