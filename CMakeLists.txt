cmake_minimum_required( VERSION 3.1 FATAL_ERROR )

project( eckit CXX )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild/cmake")

include( ecbuild_system NO_POLICY_SCOPE )

ecbuild_requires_macro_version( 2.9 )

set( PERSISTENT_NAMESPACE "eckit" CACHE INTERNAL "" ) # needed for generating .b files for persistent support

ecbuild_declare_project()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### dependencies and options

ecbuild_add_option( FEATURE EXPERIMENTAL
                    DEFAULT OFF
                    DESCRIPTION "Experimental features that havent graduated to production ready code" )

ecbuild_add_option( FEATURE SANDBOX
                    DEFAULT OFF
                    DESCRIPTION "Experimental features that havent graduated to production ready code" )

### eckit::mpi

ecbuild_add_option( FEATURE MPI
                    DEFAULT ON
                    DESCRIPTION "Use system MPI libraries"
                    REQUIRED_PACKAGES "MPI COMPONENTS C" )

### eckit::cmd

ecbuild_add_option( FEATURE ECKIT_CMD
                    DEFAULT ON
                    DESCRIPTION "Utilities for administration tools"
                    REQUIRED_PACKAGES "LEXYACC" "Curses" )

### eckit::sql

ecbuild_add_option( FEATURE ECKIT_SQL
                    DEFAULT ON
                    DESCRIPTION "An SQL engine"
                    REQUIRED_PACKAGES "LEXYACC" )

### Eigen

ecbuild_add_option( FEATURE EIGEN
                    DESCRIPTION "Eigen linear algebra library"
                    REQUIRED_PACKAGES Eigen3 )

set_package_properties( Eigen3 PROPERTIES
                        DESCRIPTION "C++ template library for linear algebra"
                      )

### RADOS

ecbuild_add_option( FEATURE RADOS
                    DEFAULT OFF
                    DESCRIPTION "Ceph/Rados storage support"
                    REQUIRED_PACKAGES Rados )

### Armadillo

ecbuild_add_option( FEATURE ARMADILLO
                    CONDITION HAVE_EXPERIMENTAL
                    DESCRIPTION "Armadillo linear algebra library"
                    REQUIRED_PACKAGES Armadillo )

set_package_properties( Armadillo PROPERTIES
                        DESCRIPTION "C++ linear algebra"
                      )

### MKL

ecbuild_add_option( FEATURE MKL
                    DESCRIPTION "MKL linear algebra library"
                    REQUIRED_PACKAGES MKL )

### Compression

ecbuild_add_option( FEATURE BZIP2
                    DESCRIPTION "BZip2 support for compression"
                    REQUIRED_PACKAGES BZip2 )

ecbuild_add_option( FEATURE SNAPPY
                    DESCRIPTION "Snappy support for compression"
                    REQUIRED_PACKAGES Snappy )

### LAPACK

if( ECKIT_HAVE_MKL )
    set( ECKIT_HAVE_LAPACK 1 )
    set( LAPACK_LIBRARIES ${MKL_LIBRARIES} )
else()
    ecbuild_add_option( FEATURE LAPACK
                        DESCRIPTION "Linear Algebra PACKage"
                        REQUIRED_PACKAGES "LAPACK QUIET" )
endif()

### OpenSSL (for SHA1)

ecbuild_add_option( FEATURE SSL
                    DESCRIPTION "OpenSSL support"
                    REQUIRED_PACKAGES OpenSSL )

if( OPENSSL_FOUND )
  ecbuild_info("OpenSSL version ${OPENSSL_VERSION} -- libs [${OPENSSL_LIBRARIES}] incs [${OPENSSL_INCLUDE_DIR}]")
endif()

### xxHash

ecbuild_add_option( FEATURE XXHASH
                    DESCRIPTION "Use xxHash for non-crypto hashing in data integrity checks"
                    REQUIRED_PACKAGES "xxHash QUIET" )

if( XXHASH_FOUND )
  ecbuild_info("xxHash -- libs [${XXHASH_LIBRARIES}] incs [${XXHASH_INCLUDE_DIRS}]")
endif()

#### Curl

ecbuild_add_option( FEATURE CURL
                    DESCRIPTION "Curl library for transfering data with URLs"
                    REQUIRED_PACKAGES CURL )

#### Jemalloc

ecbuild_add_option( FEATURE JEMALLOC
                    DEFAULT OFF
                    DESCRIPTION "Link against jemalloc memory allocator"
                    REQUIRED_PACKAGES Jemalloc )

#### CUDA

ecbuild_add_option( FEATURE CUDA
                    CONDITION HAVE_EXPERIMENTAL
                    DESCRIPTION "CUDA GPU linear algebra operations"
                    REQUIRED_PACKAGES CUDA )

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

### c math library, needed when including "math.h"

find_package( CMath )

###############################################################################
# checks

# check thread library is pthreads

if( NOT ${CMAKE_USE_PTHREADS_INIT} )
    message( FATAL_ERROR "Only pthreads supported - thread library found is [${CMAKE_THREAD_LIBS_INIT}]" )
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
add_subdirectory( doc )
add_subdirectory( src )
add_subdirectory( tests )
add_subdirectory( regressions )

ecbuild_add_resources( TARGET ${PROJECT_NAME}_top_files SOURCES
                       TODO AUTHORS README.md NOTICE LICENSE INSTALL ChangeLog COPYING )

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
