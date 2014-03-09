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

# eckit::grid

ecbuild_add_option( FEATURE GRID
					DEFAULT ON
					DESCRIPTION "eckit::grid extension [EXPERIMENTAL]" )

if( ENABLE_GRID )
	ecbuild_use_package( PROJECT atlas )
	add_feature_info( atlas ATLAS_FOUND "support for unstructured meshes")
	if( NOT ATLAS_FOUND )
		set( HAVE_GRID 0 )
	endif()
endif()

# Eigen

ecbuild_add_option( FEATURE EIGEN
					DEFAULT AUTO
					DESCRIPTION "Eigen support  [EXPERIMENTAL]"
					REQUIRED_PACKAGES Eigen3 )

set_package_properties( Eigen3 PROPERTIES TYPE RECOMMENDED PURPOSE "Accelerated dense matrix operations" )

# GRIB

ecbuild_add_option( FEATURE GRIB
					DESCRIPTION "support for GRIB format [EXPERIMENTAL]" )

if( ENABLE_GRIB )
	ecbuild_use_package( PROJECT grib_api VERSION 1.9.9 )
	if( NOT GRIB_API_FOUND )
		set( HAVE_GRIB 0 )
	endif()
endif()

set_package_properties( grib_api PROPERTIES TYPE RECOMMENDED PURPOSE "GRIB encoding and decoding" )

# eckit::maths

ecbuild_add_option( FEATURE MATHS
					DEFAULT ON
					DESCRIPTION "eckit::maths expressions [EXPERIMENTAL]" )

if( HAVE_MATHS )

	ecbuild_add_cxx11_flags()
	ecbuild_check_cxx11( PRINT )

endif()

# boost support

set(Boost_USE_MULTITHREADED      ON )
# set(Boost_DEBUG                  ON )
# set(Boost_USE_STATIC_LIBS        ON )
# set(Boost_NO_SYSTEM_PATHS        ON )

ecbuild_add_option( FEATURE BOOST
					DEFAULT ${ENABLE_MATHS}
					DESCRIPTION "Boost support [EXPERIMENTAL]"
					REQUIRED_PACKAGES "Boost 1.45.0" )

set_package_properties( Boost PROPERTIES TYPE RECOMMENDED PURPOSE "" )

#list( APPEND ECKIT_BOOST_LIBS    ${Boost_FILESYSTEM_LIBRARY}  ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} )

# MPI support

ecbuild_add_option( FEATURE MPI
					DESCRIPTION "eckit::mpi extensions [EXPERIMENTAL]" )

if( ENABLE_MPI )
	ecbuild_find_mpi()
	if( NOT MPI_CXX_FOUND )
			set( HAVE_MPI 0 )
	endif()
endif()

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

###############################################################################
# contents

ecbuild_find_project_files()

add_definitions( -D${EC_OS_NAME} )

set( ECKIT_TPLS           "" )
set( ECKIT_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src )
set( ECKIT_LIBRARIES      eckit eckit_web )

if( HAVE_GRID )
	list( APPEND ECKIT_LIBRARIES eckit_grid )
endif()

if( HAVE_GRIB )
	list( APPEND ECKIT_LIBRARIES eckit_grib )
	list( APPEND ECKIT_TPLS grib_api )
endif()

if( HAVE_EIGEN )
	list( APPEND ECKIT_TPLS Eigen3 )
endif()

get_directory_property( ECKIT_DEFINITIONS COMPILE_DEFINITIONS )

include_directories( ${ECKIT_INCLUDE_DIRS} )

# source files

add_subdirectory( src )

############################################################################################
# finalize

ecbuild_install_project( NAME eckit )

ecbuild_print_summary()
