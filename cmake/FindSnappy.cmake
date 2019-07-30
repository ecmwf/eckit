# (C) Copyright 2011- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

# - Try to find Snappy
# Once done this will define
#
#  SNAPPY_FOUND         - found Snappy
#  SNAPPY_INCLUDE_DIRS  - the Snappy include directories
#  SNAPPY_LIBRARIES     - the Snappy libraries
#
# The following paths will be searched with priority if set in CMake or env
#
#  SNAPPY_PATH          - prefix path of the Armadillo installation
#  SNAPPY_ROOT              - Set this variable to the root installation

# Search with priority for SNAPPY_PATH if given as CMake or env var

find_path(SNAPPY_INCLUDE_DIR snappy.h
          HINTS $ENV{SNAPPY_ROOT} ${SNAPPY_ROOT}
          PATHS ${SNAPPY_PATH} ENV SNAPPY_PATH
          PATH_SUFFIXES include NO_DEFAULT_PATH)

find_path(SNAPPY_INCLUDE_DIR snappy.h PATH_SUFFIXES include )

# Search with priority for SNAPPY_PATH if given as CMake or env var
find_library(SNAPPY_LIBRARY snappy
            HINTS $ENV{SNAPPY_ROOT} ${SNAPPY_ROOT}
            PATHS ${SNAPPY_PATH} ENV SNAPPY_PATH
            PATH_SUFFIXES lib64 lib NO_DEFAULT_PATH)

find_library( SNAPPY_LIBRARY snappy PATH_SUFFIXES lib64 lib )

set( SNAPPY_LIBRARIES    ${SNAPPY_LIBRARY} )
set( SNAPPY_INCLUDE_DIRS ${SNAPPY_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)

# handle the QUIET and REQUIRED arguments and set SNAPPY_FOUND to TRUE
# if all listed variables are TRUE
# Note: capitalisation of the package name must be the same as in the file name
find_package_handle_standard_args(Snappy DEFAULT_MSG SNAPPY_LIBRARY SNAPPY_INCLUDE_DIR)

mark_as_advanced(SNAPPY_INCLUDE_DIR SNAPPY_LIBRARY)
