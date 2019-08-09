# (C) Copyright 2011- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

# - Try to find xxHash
# Once done this will define
#
#  XXHASH_FOUND         - found xxHash
#  XXHASH_INCLUDE_DIRS  - the xxHash include directories
#  XXHASH_LIBRARIES     - the xxHash libraries
#
# The following paths will be searched with priority if set in CMake or env
#
#  XXHASH_PATH          - prefix path of the Armadillo installation
#  XXHASH_ROOT              - Set this variable to the root installation

# Search with priority for XXHASH_PATH if given as CMake or env var

if(NOT XXHASH_FOUND)

find_path(XXHASH_INCLUDE_DIR xxhash.h
        HINTS $ENV{XXHASH_ROOT} ${XXHASH_ROOT}
        PATHS ${XXHASH_PATH} ENV XXHASH_PATH
        PATH_SUFFIXES include include/xxhash NO_DEFAULT_PATH)

find_path(XXHASH_INCLUDE_DIR xxhash.h PATH_SUFFIXES include include/xxhash )

# Search with priority for XXHASH_PATH if given as CMake or env var
find_library(XXHASH_LIBRARY xxhash
        HINTS $ENV{XXHASH_ROOT} ${XXHASH_ROOT}
        PATHS ${XXHASH_PATH} ENV XXHASH_PATH
        PATH_SUFFIXES lib64 lib NO_DEFAULT_PATH)

find_library( XXHASH_LIBRARY xxhash PATH_SUFFIXES lib64 lib )

set( XXHASH_LIBRARIES    ${XXHASH_LIBRARY} )
set( XXHASH_INCLUDE_DIRS ${XXHASH_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)

# handle the QUIET and REQUIRED arguments and set XXHASH_FOUND to TRUE
# if all listed variables are TRUE
# Note: capitalisation of the package name must be the same as in the file name
find_package_handle_standard_args(xxHash DEFAULT_MSG XXHASH_LIBRARY XXHASH_INCLUDE_DIR)

mark_as_advanced(XXHASH_INCLUDE_DIR XXHASH_LIBRARY)

endif()