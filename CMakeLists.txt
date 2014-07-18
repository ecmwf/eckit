############################################################################################
#
#   eckit - ECMWF C++ Toolkit
#
### TODO
#
#   * remove architecture dependent headers
#
#   * add control of behavior on Exception
#       - output on constructor
#       - backtrace on contructor
#       - abort on constructor
#       - abort on double exception
#
#   * new Logging
#       - allow finer control on format ( eg [%time][%pid] message )

cmake_minimum_required( VERSION 2.8.4 FATAL_ERROR )

project( eckit CXX )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild/cmake")

include( ecbuild_system )

ecbuild_requires_macro_version( 1.3 )

set( PERSISTENT_NAMESPACE eckit CACHE INTERNAL "" )

###############################################################################
# local project

ecbuild_declare_project()

###############################################################################
# some variables/options of this project

# Eigen

ecbuild_add_option( FEATURE EIGEN
					DEFAULT OFF
					DESCRIPTION "Eigen support  [EXPERIMENTAL]"
					REQUIRED_PACKAGES Eigen3 )

set_package_properties( Eigen3 PROPERTIES TYPE RECOMMENDED PURPOSE "Accelerated dense matrix operations" )

# GRIB

ecbuild_add_option( FEATURE GRIB
					DEFAULT OFF
					DESCRIPTION "support for GRIB format [EXPERIMENTAL]"
					REQUIRED_PACKAGES "PROJECT grib_api VERSION 1.9.9" )

set_package_properties( grib_api PROPERTIES TYPE RECOMMENDED PURPOSE "GRIB encoding and decoding" )

# eckit::maths

ecbuild_add_option( FEATURE MATHS
                    DEFAULT OFF
                    DESCRIPTION "eckit::maths expressions [EXPERIMENTAL]"
                    REQUIRED_PACKAGES Eigen3 )

if( HAVE_MATHS )
	ecbuild_add_cxx11_flags() # enforce c++11
endif()

ecbuild_check_cxx11( PRINT ) # always test for c++11

# async io support

find_package( AIO )
set_package_properties( AIO PROPERTIES TYPE RECOMMENDED PURPOSE "support for asynchronous IO" )

# find thread library ( preferably pthreads )

set( CMAKE_THREAD_PREFER_PTHREAD 1 )
find_package(Threads REQUIRED)

###############################################################################
# checks

# check thread library is pthreads

if( NOT ${CMAKE_USE_PTHREADS_INIT} )
	message( FATAL_ERROR "Only pthreads supported - thread library found is [${CMAKE_THREAD_LIBS_INIT}]" )
endif()

# fail if we dont have bool
if( NOT EC_HAVE_CXX_BOOL )
	message( FATAL_ERROR "eckit cannot build -- c++ compiler does not support bool" )
endif()
# fail if we dont have sstream
if( NOT EC_HAVE_CXX_SSTREAM )
	message( FATAL_ERROR "eckit cannot build -- c++ compiler does not support stringstream" )
endif()

############################################################################################
# export package info

set( ECKIT_PACKAGES       "" )
set( ECKIT_TPLS           "" )
set( ECKIT_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src )
set( ECKIT_LIBRARIES      eckit eckit_web eckit_geometry )

if( HAVE_GRIB )
  list( APPEND ECKIT_PACKAGES ECKIT_GRIB )
  list( APPEND ECKIT_LIBRARIES eckit_grib )
  list( APPEND ECKIT_TPLS grib_api )
endif()

if( HAVE_MATHS )
  list( APPEND ECKIT_PACKAGES ECKIT_MATHS )
  list( APPEND ECKIT_LIBRARIES eckit_maths )
  list( APPEND ECKIT_TPLS Eigen3 )
endif()

get_directory_property( ECKIT_DEFINITIONS COMPILE_DEFINITIONS )

############################################################################################
# sources

ecbuild_find_project_files()

include_directories( ${ECKIT_INCLUDE_DIRS} )

add_subdirectory( src )

############################################################################################
# finalize

ecbuild_install_project( NAME eckit )

ecbuild_print_summary()
