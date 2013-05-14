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
#       - allows reconfiguration
#       - allows control on format ( eg [%time][%pid] message )
#       - always available (not dependent on ContextBehavior or have default)
#       - simpler ?

cmake_minimum_required( VERSION 2.8.4 FATAL_ERROR )

project( eckit CXX )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

include( ecbuild_system )

if( ECBUILD_MACRO_VERSION VERSION_LESS 1.1 )
    message( FATAL_ERROR "${PROJECT_NAME} needs ecbuild >= 1.1" )
endif() 

set( PERSISTENT_NAMESPACE eckit CACHE INTERNAL "" )

###############################################################################
# local project

ecbuild_declare_project()

###############################################################################
# some variables/options of this project

set(Boost_USE_MULTITHREADED        ON )
# set(Boost_DEBUG                  ON )
# set(Boost_USE_STATIC_LIBS        ON )
# set(Boost_NO_SYSTEM_PATHS        ON )

find_package( Boost 1.50.0 COMPONENTS filesystem program_options date_time thread system unit_test_framework test_exec_monitor )

list( APPEND ECKIT_BOOST_LIBS      ${Boost_FILESYSTEM_LIBRARY}  ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY} )

find_program( PSTACK_COMMAND "pstack" DOC "path to the pstack executable" )
mark_as_advanced(PSTACK_COMMAND)

find_program( CPPFILT_COMMAND "c++filt" DOC "path to the c++filt executable" )
mark_as_advanced(CPPFILT_COMMAND)

###############################################################################
# macro processing

###############################################################################
# contents

ecbuild_find_project_files()

add_definitions( -D${EC_OS_NAME} )

set( ECKIT_TPLS           "" )
set( ECKIT_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src )
set( ECKIT_LIBRARIES      eckit )

get_directory_property( COMPILE_DEFINITIONS ECKIT_DEFINITIONS )
    
debug_var( ECKIT_INCLUDE_DIRS )
debug_var( ECKIT_DEFINITIONS )

include_directories( ${ECKIT_INCLUDE_DIRS} )

# source files

add_subdirectory( src )

############################################################################################
# finalize

ecbuild_install_project( NAME eckit )

ecbuild_print_summary()
