# (C) Copyright 2011- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

# - Try to find libLZ4
# Once done this will define
#
#  LZ4_FOUND         - found LZ4
#  LZ4_INCLUDE_DIRS  - the LZ4 include directories
#  LZ4_LIBRARIES     - the LZ4 libraries
#
# The following paths will be searched with priority if set in CMake or env
#
#  LZ4_PATH          - prefix path of the Armadillo installation
#  LZ4_ROOT              - Set this variable to the root installation

# Search with priority for LZ4_PATH if given as CMake or env var

find_path(LZ4_INCLUDE_DIR lz4.h
          HINTS $ENV{LZ4_ROOT} ${LZ4_ROOT}
          PATHS ${LZ4_PATH} ENV LZ4_PATH
          PATH_SUFFIXES include NO_DEFAULT_PATH)

find_path(LZ4_INCLUDE_DIR lz4.h PATH_SUFFIXES include )

# Search with priority for LZ4_PATH if given as CMake or env var
find_library(LZ4_LIBRARY lz4
            HINTS $ENV{LZ4_ROOT} ${LZ4_ROOT}
            PATHS ${LZ4_PATH} ENV LZ4_PATH
            PATH_SUFFIXES lib64 lib NO_DEFAULT_PATH)

find_library( LZ4_LIBRARY lz4 PATH_SUFFIXES lib64 lib )

set( LZ4_LIBRARIES    ${LZ4_LIBRARY} )
set( LZ4_INCLUDE_DIRS ${LZ4_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)

# handle the QUIET and REQUIRED arguments and set LZ4_FOUND to TRUE
# if all listed variables are TRUE
# Note: capitalisation of the package name must be the same as in the file name
find_package_handle_standard_args(LZ4 DEFAULT_MSG LZ4_LIBRARY LZ4_INCLUDE_DIR)

mark_as_advanced(LZ4_INCLUDE_DIR LZ4_LIBRARY)
