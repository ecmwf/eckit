# (C) Copyright 2011- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

########################################################################################################################

cmake_minimum_required( VERSION 3.12 FATAL_ERROR )

find_package( ecbuild 3.5 REQUIRED HINTS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild)

project( eckit LANGUAGES CXX )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

########################################################################################################################
### dependencies and options

ecbuild_add_option( FEATURE BUILD_TOOLS
                    DEFAULT ON
                    DESCRIPTION "Build the command line tools" )

ecbuild_add_option( FEATURE EXPERIMENTAL
                    DEFAULT OFF
                    DESCRIPTION "Experimental features that havent graduated to production ready code" )

ecbuild_add_option( FEATURE SANDBOX
                    DEFAULT OFF
                    DESCRIPTION "Sandbox playground for prototyping code that may never see the light of day" )

### thread library ( preferably pthreads ) --- Must be called before FindCUDA!

if( ${CMAKE_VERSION} VERSION_LESS 3.14 )
    # Following variable is unused for CMake > 3.14
    set( CMAKE_THREAD_PREFER_PTHREAD 1 )
endif()

if( NOT DEFINED THREADS_PREFER_PTHREAD_FLAG )
  set( THREADS_PREFER_PTHREAD_FLAG 1 )
endif()

### Set THREADS_HAVE_PTHREAD_ARG variable to false so that it can be recomputed based on
### THREADS_PREFER_PTHREAD_FLAG, in case other project had it on a different setting.
### This is certainly a CMake bug ( see ECKIT-426 )
set( THREADS_HAVE_PTHREAD_ARG FALSE )
find_package( Threads REQUIRED )
set( THREADS_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} )

### eckit::mpi

ecbuild_add_option( FEATURE MPI
                    DEFAULT ON
                    DESCRIPTION "Use system MPI libraries"
                    REQUIRED_PACKAGES "MPI COMPONENTS C" )

### eckit::cmd

ecbuild_find_lexyacc()

ecbuild_add_option( FEATURE ECKIT_CMD
                    DEFAULT ON
                    DESCRIPTION "Utilities for administration tools"
                    CONDITION LEXYACC_FOUND
                    REQUIRED_PACKAGES "Curses" )

### eckit::sql

ecbuild_add_option( FEATURE ECKIT_SQL
                    DEFAULT ON
                    DESCRIPTION "An SQL engine"
                    CONDITION LEXYACC_FOUND )

### Eigen

ecbuild_add_option( FEATURE EIGEN
                    DESCRIPTION "Eigen linear algebra library"
                    REQUIRED_PACKAGES Eigen3 )

set_package_properties( Eigen3 PROPERTIES
                        DESCRIPTION "C++ template library for linear algebra"
                      )

### OpenMP

ecbuild_add_option( FEATURE OMP
                    DEFAULT OFF
                    DESCRIPTION "OpenMP linear algebra backend"
                    REQUIRED_PACKAGES "OpenMP COMPONENTS CXX" )

if( NOT TARGET OpenMP::OpenMP_CXX )
    set( eckit_HAVE_OMP 0 )
endif()

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

### Compression options

ecbuild_add_option( FEATURE BZIP2
                    DESCRIPTION "BZip2 support for compression"
                    REQUIRED_PACKAGES BZip2 )

ecbuild_add_option( FEATURE SNAPPY
                    DESCRIPTION "Snappy support for compression"
                    REQUIRED_PACKAGES Snappy )

ecbuild_add_option( FEATURE LZ4
                    DESCRIPTION "LZ4 support for compression"
                    REQUIRED_PACKAGES LZ4 )

ecbuild_add_option( FEATURE AEC
                    DESCRIPTION "AEC support for compression"
                    REQUIRED_PACKAGES AEC )

### Hashing options

ecbuild_add_option( FEATURE XXHASH
                    DESCRIPTION "xxHash support for hashing" )

### LAPACK

if( eckit_HAVE_MKL )
    set( eckit_HAVE_LAPACK 1 )
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

#### Curl

ecbuild_add_option( FEATURE CURL
                    DESCRIPTION "Curl library for transfering data with URLs"
                    REQUIRED_PACKAGES "CURL VERSION 7.20" )

if(HAVE_CURL)
  ecbuild_info("Curl version ${CURL_VERSION_STRING} -- libs [${CURL_LIBRARIES}] incs [${CURL_INCLUDE_DIRS}]")
endif()

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

### LibRsync

ecbuild_add_option( FEATURE RSYNC
                    DEFAULT OFF
                    DESCRIPTION "librsync implementation of the rsync algorithm"
                    REQUIRED_PACKAGES LibRsync )
if( HAVE_RSYNC )
  set( LIBRSYNC_LIBRARIES rsync )
endif()

ecbuild_add_option( FEATURE SANDBOX
                    DEFAULT OFF
                    DESCRIPTION "Sandbox playground for prototyping code that may never see the light of day" )

### Performance tests

ecbuild_add_option( FEATURE EXTRA_TESTS
                    DEFAULT OFF
                    DESCRIPTION "Add additional unit/performance tests" )

### UNICODE support

check_include_file_cxx("codecvt" HAVE_CXX_CODECVT)

ecbuild_add_option( FEATURE UNICODE
                    DEFAULT ON
                    CONDITION HAVE_CXX_CODECVT
                    DESCRIPTION "Add support for Unicode characters in Yaml/JSON parsers" )

### async io support

find_package( AIO )
ecbuild_add_option( FEATURE AIO
                    DEFAULT ON
                    CONDITION ${AIO_FOUND}
                    DESCRIPTION "support for asynchronous IO")

### c math library, needed when including "math.h"

find_package( CMath )

### realtime library implements shm_open on Linux
if( CMAKE_SYSTEM_NAME MATCHES "Linux" )
    find_package( Realtime )
    if( REALTIME_FOUND ) # check that aio needs realtime
        set( RT_LIBRARIES ${RT_LIB} )
    endif()
else()
  set( RT_LIBRARIES "" )
endif()

### sanity checks

include(cmake/check_time_t_Y2038.cmake) # check Y2038 time_t compatibility

# check thread library is pthreads
if( NOT ${CMAKE_USE_PTHREADS_INIT} )
    message( FATAL_ERROR "Only pthreads supported - thread library found is [${THREADS_LIBRARIES}]" )
endif()

############################################################################################
# sources

include(cmake/compiler_warnings.cmake) # optionally handle compiler specific warnings

set( PERSISTENT_NAMESPACE "eckit" CACHE INTERNAL "" ) # needed for generating .b files for persistent support

add_subdirectory( src )

add_subdirectory( bamboo )
add_subdirectory( doc )
add_subdirectory( tests )
add_subdirectory( regressions )

ecbuild_add_resources( TARGET ${PROJECT_NAME}_top_files
					   SOURCES AUTHORS README.md NOTICE LICENSE
					   INSTALL ChangeLog COPYING )

############################################################################################
# finalize

set( ECKIT_LIBRARIES ${eckit_ALL_LIBS} ) # used for ecbuild_pkgconfig

foreach( _lib ${ECKIT_LIBRARIES} )
    ecbuild_pkgconfig( NAME ${_lib}
                       DESCRIPTION "ECMWF C++ Toolkit - ${_lib} library"
                       URL "https://software.ecmwf.int/wiki/display/ECKIT/ecKit"
                       LIBRARIES ${_lib} )
endforeach()

ecbuild_install_project( NAME ${PROJECT_NAME} )

ecbuild_print_summary()
