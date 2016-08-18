########################################################################################################################
#
#   eckit - ECMWF C++ Toolkit
#
########################################################################################################################

cmake_minimum_required( VERSION 2.8.11 FATAL_ERROR )

project( eckit CXX )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild/cmake")

include( ecbuild_system NO_POLICY_SCOPE )

ecbuild_requires_macro_version( 2.0 )

set( PERSISTENT_NAMESPACE eckit CACHE INTERNAL "" )

###############################################################################
# local project

ecbuild_declare_project()

###############################################################################
# some variables/options of this project

### Experimental features

ecbuild_add_option( FEATURE EXPERIMENTAL
                    DEFAULT ON
                    DESCRIPTION "Experimental features (not used in production)"
                    PURPOSE "Enables MPI, MPISTUBS, ECKIT_MPI, ECKIT_MPISTUBS" )

### eckit::mpi

debug_var(ENABLE_MPI)

ecbuild_add_option( FEATURE MPI
                    DEFAULT ON
                    DESCRIPTION "Use system MPI libraries"
                    PURPOSE "Support for MPI distributed parallelism"
                    REQUIRED_PACKAGES "MPI COMPONENTS CXX" )

                debug_var(HAVE_MPI)
                debug_var(HAVE_ECKIT_MPI)

### eckit::cmd

ecbuild_add_option( FEATURE ECKIT_CMD
                    DESCRIPTION "Utilities for administration tools"
                    PURPOSE "Build eckit_cmd library"
                    REQUIRED_PACKAGES "LEXYACC" )

### Eigen

ecbuild_add_option( FEATURE EIGEN
                    DESCRIPTION "Eigen support"
                    TYPE RECOMMENDED
                    PURPOSE "Eigen backend: accelerated dense and sparse matrix operations"
                    REQUIRED_PACKAGES Eigen3 )

set_package_properties( Eigen3 PROPERTIES
                        TYPE RECOMMENDED
                        DESCRIPTION "C++ template library for linear algebra"
                        PURPOSE "Dense and sparse matrix operations" )

if( ENABLE_EIGEN )
    # TODO: we should add here a test that we can compile Eigen without problems
    # TODO: don't use contrib just yet -- sort out the legal stuff first
    # set( ECKIT_CONTRIB_EIGEN 1 )
endif()

### Armadillo

ecbuild_add_option( FEATURE ARMADILLO
                    CONDITION HAVE_EXPERIMENTAL
                    DESCRIPTION "Armadillo linear algebra library"
                    TYPE RECOMMENDED
                    PURPOSE "Armadillo backend: accelerated dense matrix operations"
                    REQUIRED_PACKAGES Armadillo )

set_package_properties( Armadillo PROPERTIES
                        DESCRIPTION "C++ linear algebra"
                        TYPE RECOMMENDED
                        PURPOSE "Dense matrix operations" )

### LAPACK

if( NOT ( BUILD_SITE MATCHES "cc[abt].*" ) )
  ecbuild_add_option( FEATURE LAPACK
                      DESCRIPTION "Linear Algebra PACKage"
                      REQUIRED_PACKAGES "PROJECT LAPACK" )
  if( HAVE_LAPACK )
    if( TARGET lapack )
      ecbuild_debug( "LAPACK found as CMake target lapack" )
      set( LAPACK_LIBRARIES lapack )
    endif()
  endif()
  ecbuild_debug_var( LAPACK_LIBRARIES )
# Don't search for LAPACK on cc[abt] as it is provided by the cray-libsci module
# which is loaded by default
else()
  set( _cray_libsci_loaded $ENV{CRAY_LIBSCI_DIR} )
  ecbuild_add_option( FEATURE LAPACK
                      CONDITION _cray_libsci_loaded
                      DESCRIPTION "Linear Algebra PACKage")
  if( HAVE_LAPACK )
    ecbuild_debug( "LAPACK found, already loaded as part of Cray's libsci" )
  endif()
endif()

### CUDA

ecbuild_add_option( FEATURE CUDA
                    CONDITION HAVE_EXPERIMENTAL
                    DESCRIPTION "CUDA GPU support"
                    TYPE RECOMMENDED
                    PURPOSE "CUDA backend: GPU linear algebra operations"
                    REQUIRED_PACKAGES CUDA )

### MKL

ecbuild_add_option( FEATURE MKL
                    CONDITION HAVE_EXPERIMENTAL
                    DESCRIPTION "MKL support"
                    TYPE RECOMMENDED
                    PURPOSE "MKL backend: Accelerated linear algebra operations"
                    REQUIRED_PACKAGES MKL )

### ViennaCL

ecbuild_add_option( FEATURE VIENNACL
                    CONDITION HAVE_EXPERIMENTAL
                    DESCRIPTION "ViennaCL support for OpenCL"
                    TYPE RECOMMENDED
                    PURPOSE "ViennaCL backend: many-core linear algebra library"
                    REQUIRED_PACKAGES ViennaCL )

set_package_properties( ViennaCL PROPERTIES
                        DESCRIPTION "linear algebra library for computations on many-core architectures"
                        TYPE RECOMMENDED
                        PURPOSE "Dense and sparse matrix operations on OpenCL devices" )

### async io support

find_package( AIO )
set_package_properties( AIO PROPERTIES TYPE RECOMMENDED PURPOSE "support for asynchronous IO" )

### thread library ( preferably pthreads )

set( CMAKE_THREAD_PREFER_PTHREAD 1 )
find_package( Threads REQUIRED )

### Curses (for CmdApplication)

find_package( Curses REQUIRED )

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

set( ECKIT_INCLUDE_DIRS   ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src )
set( ECKIT_LIBRARIES      eckit eckit_geometry eckit_linalg eckit_maths eckit_web )

if( HAVE_ECKIT_CMD)
  list( APPEND ECKIT_LIBRARIES eckit_cmd )
endif()

if( HAVE_ECKIT_MPI )
  list( APPEND ECKIT_LIBRARIES eckit_mpi )
endif()

if( HAVE_EIGEN AND ECKIT_CONTRIB_EIGEN )
  list( APPEND ECKIT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/contrib )
endif()

if( HAVE_EXPERIMENTAL )

  list( APPEND ECKIT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/experimental )

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

ecbuild_add_resources( TARGET ${PROJECT_NAME}_top_files SOURCES
                       TODO AUTHORS README NOTICE LICENSE INSTALL ChangeLog COPYING )

############################################################################################
# finalize

foreach( _lib ${ECKIT_LIBRARIES} )
  if( TARGET ${_lib} )
    ecbuild_pkgconfig( NAME ${_lib}
                       DESCRIPTION "ECMWF C++ Toolkit - ${_lib} library"
                       URL "https://software.ecmwf.int/wiki/display/ECKIT/ecKit"
                       LIBRARIES ${_lib} )
  endif()
endforeach()

ecbuild_install_project( NAME ${PROJECT_NAME} )

ecbuild_print_summary()
