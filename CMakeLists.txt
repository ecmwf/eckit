############################################################################################
#
#   eckit - ECMWF C++ Toolkit
#
### TODO
#
#   * remove architecture dependent headers
#   * remove using std::
#
#   * add control of behavior on Exception
#       - output on constructor
#       - backtrace on contructor
#       - abort on constructor
#       - abort on double exception
#
#   * new Logging
#       - allows control on format ( eg [%time][%pid] message )

cmake_minimum_required( VERSION 2.8.4 FATAL_ERROR )

project( eckit CXX )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

include( ecbuild_system )

ecbuild_requires_macro_version( 1.2 )

set( PERSISTENT_NAMESPACE eckit CACHE INTERNAL "" )

###############################################################################
# local project

ecbuild_declare_project()

###############################################################################
# some variables/options of this project

option( ECKIT_BOOST  "Boost support  [NOT IMPLEMENTED]"		        OFF )
option( ECKIT_MATHS  "eckit::maths extension [EXPERIMENTAL]"        OFF )
option( ECKIT_GRID   "eckit::grid extension [EXPERIMENTAL]"	        OFF )
option( ECKIT_GRIB   "eckit::grib extension [EXPERIMENTAL]"	        OFF )
option( ECKIT_MPI    "eckit::mpi  extension [EXPERIMENTAL]"	        OFF )
option( ECKIT_EIGEN  "Eigen support  [NOT IMPLEMENTED]"		        OFF )

if( ECKIT_GRIB )
	ecbuild_use_package( PROJECT grib_api VERSION 1.9.9 )
endif()

if( ECKIT_EIGEN)
	find_package(Eigen3)
endif()

if( ECKIT_MATHS AND CMAKE_COMPILER_IS_GNUCXX )
	set( ECKIT_BOOST ON )
	if( CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.7 )
		cmake_add_cxx_flags("-std=c++0x")
	else()
		cmake_add_cxx_flags("-std=c++11")
	endif()
endif()

# boost support

if( ECKIT_BOOST )
	ecbuild_add_extra_search_paths( boost )

	set(Boost_USE_MULTITHREADED      ON )
	# set(Boost_DEBUG                  ON )
	# set(Boost_USE_STATIC_LIBS        ON )
	# set(Boost_NO_SYSTEM_PATHS        ON )

	#find_package( Boost 1.45.0 COMPONENTS filesystem program_options date_time thread system unit_test_framework test_exec_monitor )
	find_package( Boost 1.45.0 )

	#list( APPEND ECKIT_BOOST_LIBS    ${Boost_FILESYSTEM_LIBRARY}  ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} )
endif()

# stack inspection

find_program( PSTACK_COMMAND "pstack" DOC "path to the pstack executable" )
mark_as_advanced(PSTACK_COMMAND)

find_program( CPPFILT_COMMAND "c++filt" DOC "path to the c++filt executable" )
mark_as_advanced(CPPFILT_COMMAND)

# MPI support

find_package( MPI )

if( NOT MPI_CXX_FOUND )
	set( ECKIT_MPI OFF )
endif()

###############################################################################
# checks

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

if( ECKIT_GRID )
	list( APPEND ECKIT_LIBRARIES eckit_grid )
endif()

if( ECKIT_GRIB AND GRIB_API_FOUND )
	list( APPEND ECKIT_LIBRARIES eckit_grib )
	list( APPEND ECKIT_TPLS grib_api )
endif()

if( ECKIT_EIGEN3 AND EIGEN3_FOUND )
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
