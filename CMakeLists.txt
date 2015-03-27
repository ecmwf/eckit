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

ecbuild_requires_macro_version( 1.7 )

set( PERSISTENT_NAMESPACE eckit CACHE INTERNAL "" )

###############################################################################
# local project

ecbuild_declare_project()

###############################################################################
# some variables/options of this project

### Check for C++11 support

list( APPEND cpp11_req
      auto constexpr constructor_delegate cstdint decltype deleted_constructor
      initialization lambda long_double long_long noexcept nullptr override
      rvalue_references shared_ptr sizeof_member static_assert template_alias
      type_traits unique_ptr variadic_templates )

# currently not included: __func__ random_device trivial_type_traits explicit_cast enable_if

ecbuild_check_cxx11( PRINT FEATURES ${cpp11_req} )

# If any features are not supported
if( NOT CXX11_NOT_SUPPORTED_FEATURES )
    set( HAVE_CPP11 ON )
    message( STATUS "Required C++11 features detected" )
    message( STATUS "  enabling C++11 support" )
else()
    message( STATUS "Required C++11 features not detected" )
    message( STATUS "  disabling C++11 support" )
endif()

### GRIB

ecbuild_add_option( FEATURE GRIB
                    DEFAULT ON
                    DESCRIPTION "C++ interface to GRIB format"
                    REQUIRED_PACKAGES "PROJECT grib_api VERSION 1.12.3" )

set_package_properties( grib_api PROPERTIES TYPE RECOMMENDED PURPOSE "GRIB encoding and decoding" )

### eckit::xpr

ecbuild_add_option( FEATURE XPR
                    DEFAULT OFF
                    DESCRIPTION "eckit::xpr [EXPERIMENTAL]"
                    CONDITION HAVE_CPP11 )

### eckit::mpi

ecbuild_add_option( FEATURE MPI
                    DESCRIPTION "Support for MPI distributed parallelism"
                    REQUIRED_PACKAGES "MPI COMPONENTS CXX" )

ecbuild_add_option( FEATURE MPISTUBS
                    DESCRIPTION "Support for MPI distributed parallelism"
                    REQUIRED_PACKAGES "PROJECT mpistubs VERSION 1.0.0" )

ecbuild_add_option( FEATURE ECKIT_MPI
                    DESCRIPTION "eckit C++ wrapper for mpi"
                    CONDITION HAVE_MPI )

ecbuild_add_option( FEATURE ECKIT_MPISTUBS
                    DESCRIPTION "eckit C++ wrapper for mpi using mpistubs"
                    CONDITION HAVE_MPISTUBS )

### Eigen

ecbuild_add_option( FEATURE EIGEN
                    DEFAULT ON
                    DESCRIPTION "Eigen support"
                    REQUIRED_PACKAGES Eigen3 )

set_package_properties( Eigen3 PROPERTIES TYPE RECOMMENDED PURPOSE "Accelerated dense matrix operations" )

if( ENABLE_EIGEN )
    # TODO: we should add here a test that we can compile Eigen without problems
    # TODO: don't use contrib just yet -- sort out the legal stuff first
    # set( ECKIT_CONTRIB_EIGEN 1 )
endif()

### LAPACK

if( NOT ( BUILD_SITE MATCHES "cc[abt].*" ) )
  ecbuild_add_option( FEATURE LAPACK
                      DESCRIPTION "Linear Algebra PACKage"
                      REQUIRED_PACKAGES LAPACK )
else() # Don't search for it on cc[abt] as it is already available as scilib
  ecbuild_add_option( FEATURE LAPACK
                      DESCRIPTION "Linear Algebra PACKage")
endif()

### async io support

find_package( AIO )
set_package_properties( AIO PROPERTIES TYPE RECOMMENDED PURPOSE "support for asynchronous IO" )

### thread library ( preferably pthreads )

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

set( ECKIT_TPLS           "" )
set( ECKIT_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src )
set( ECKIT_LIBRARIES      eckit eckit_web eckit_maths eckit_geometry )

if( HAVE_GRIB )
  list( APPEND ECKIT_LIBRARIES eckit_grib )
  list( APPEND ECKIT_TPLS grib_api )
endif()

if( HAVE_EIGEN )
    if( ECKIT_CONTRIB_EIGEN )
        list( APPEND ECKIT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/contrib )
    else()
        list( APPEND ECKIT_TPLS Eigen3 )

    endif()
endif()

if( HAVE_XPR )
  list( APPEND ECKIT_LIBRARIES eckit_xpr )
endif()

if( HAVE_ECKIT_MPI )
  list( APPEND ECKIT_LIBRARIES eckit_mpi )
endif()

if( HAVE_ECKIT_MPISTUBS )
  list( APPEND ECKIT_LIBRARIES eckit_mpistubs )
  list( APPEND ECKIT_TPLS mpistubs )
endif()

get_directory_property( ECKIT_DEFINITIONS COMPILE_DEFINITIONS )

############################################################################################
# sources

ecbuild_find_project_files()

include_directories( ${ECKIT_INCLUDE_DIRS} )
if( HAVE_EIGEN )
    include_directories( ${EIGEN3_INCLUDE_DIR} )
endif()

add_subdirectory( bamboo )
add_subdirectory( contrib )
add_subdirectory( doc )
add_subdirectory( src )

############################################################################################
# finalize

ecbuild_pkgconfig( NAME ${PROJECT_NAME}
                   DESCRIPTION "ECMWF C++ Toolkit"
                   URL "https://software.ecmwf.int/wiki/display/ECKIT/ecKit"
                   LIBRARIES eckit )

ecbuild_install_project( NAME ${PROJECT_NAME} )

ecbuild_print_summary()
