########################################################################################################################
#
#   eckit - ECMWF C++ Toolkit
#
########################################################################################################################

cmake_minimum_required( VERSION 2.8.11 FATAL_ERROR )

project( eckit CXX )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild/cmake")

include( ecbuild_system NO_POLICY_SCOPE )

ecbuild_requires_macro_version( 2.4 )

set( PERSISTENT_NAMESPACE "eckit" CACHE INTERNAL "" ) # needed for generating .b files for persistent support

###############################################################################
# local project

ecbuild_declare_project()

###############################################################################
# some variables/options of this project

### C++11 compiler flag

ecbuild_add_option( FEATURE CXX11
                    DEFAULT OFF
                    DESCRIPTION "Use C++11 compiler flag" )

### Experimental features

ecbuild_add_option( FEATURE EXPERIMENTAL
                    DEFAULT ON
                    DESCRIPTION "Experimental features Armadillo, CUDA, MKL, ViennaCL (not used in production)" )

### eckit::mpi

ecbuild_add_option( FEATURE MPI
                    DEFAULT ON
                    DESCRIPTION "Use system MPI libraries"
                    REQUIRED_PACKAGES "MPI COMPONENTS CXX" )

### eckit::cmd

ecbuild_add_option( FEATURE ECKIT_CMD
                    DESCRIPTION "Utilities for administration tools"
                    REQUIRED_PACKAGES "LEXYACC" )

### Eigen

ecbuild_add_option( FEATURE EIGEN
                    DESCRIPTION "Eigen linear algebra library"
                    REQUIRED_PACKAGES Eigen3 )

set_package_properties( Eigen3 PROPERTIES
                        DESCRIPTION "C++ template library for linear algebra"
                      )

if( ENABLE_EIGEN )
    # TODO: we should add here a test that we can compile Eigen without problems
    # TODO: don't use contrib just yet -- sort out the legal stuff first
    # set( ECKIT_CONTRIB_EIGEN 1 )
endif()

### Armadillo

ecbuild_add_option( FEATURE ARMADILLO
                    CONDITION HAVE_EXPERIMENTAL
                    DESCRIPTION "Armadillo linear algebra library"
                    REQUIRED_PACKAGES Armadillo )

set_package_properties( Armadillo PROPERTIES
                        DESCRIPTION "C++ linear algebra"
                      )

### LAPACK

ecbuild_add_option( FEATURE LAPACK
                    DESCRIPTION "Linear Algebra PACKage"
                    REQUIRED_PACKAGES "LAPACK" )

### OpenSSL (for SHA1)

ecbuild_add_option( FEATURE SSL
                    DESCRIPTION "OpenSSL support"
                    REQUIRED_PACKAGES OpenSSL )

ecbuild_info("OpenSSL ${OPENSSL_FOUND} ${OPENSSL_VERSION} : libs ${OPENSSL_LIBRARIES} incs ${OPENSSL_INCLUDE_DIR}")

### xxHash

ecbuild_add_option( FEATURE XXHASH
                    DESCRIPTION "Use xxHash for non-crypto hashing in data integrity checks"
                    REQUIRED_PACKAGES "PROJECT xxHash" )

ecbuild_info("xxHash ${XXHASH_FOUND}  : libs ${XXHASH_LIBRARIES} incs ${XXHASH_INCLUDE_DIRS}")

#### CUDA

ecbuild_add_option( FEATURE CUDA
                    CONDITION HAVE_EXPERIMENTAL
                    DESCRIPTION "CUDA GPU linear algebra operations"
                    REQUIRED_PACKAGES CUDA )

### MKL

ecbuild_add_option( FEATURE MKL
                    CONDITION HAVE_EXPERIMENTAL
                    DESCRIPTION "MKL linear algebra library"
                    REQUIRED_PACKAGES MKL )

### ViennaCL

ecbuild_add_option( FEATURE VIENNACL
                    CONDITION HAVE_EXPERIMENTAL
                    DESCRIPTION "ViennaCL OpenCL linear algebra operations"
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

### c math library, needed when including "math.h"

find_package( CMath )

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

if( HAVE_CXX11 )
    ecbuild_add_cxx11_flags()
endif()

add_subdirectory( bamboo )
add_subdirectory( contrib )
add_subdirectory( doc )
add_subdirectory( src )
add_subdirectory( regressions )

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
