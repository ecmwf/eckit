# (C) Copyright 2011- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

# - Try to find libAEC
# Once done this will define
#
#  AEC_FOUND         - found AEC
#  AEC_INCLUDE_DIRS  - the AEC include directories
#  AEC_LIBRARIES     - the AEC libraries
#
# The following paths will be searched with priority if set in CMake or env
#
#  AEC_PATH          - prefix path of the Armadillo installation
#  AEC_ROOT              - Set this variable to the root installation

# Search with priority for AEC_PATH if given as CMake or env var

find_path(AEC_INCLUDE_DIR libaec.h
          HINTS $ENV{AEC_ROOT} ${AEC_ROOT}
          PATHS ${AEC_PATH} ENV AEC_PATH
          PATH_SUFFIXES include NO_DEFAULT_PATH)

find_path(AEC_INCLUDE_DIR libaec.h PATH_SUFFIXES include )

# Search with priority for AEC_PATH if given as CMake or env var
find_library(AEC_LIBRARY aec
            HINTS $ENV{AEC_ROOT} ${AEC_ROOT}
            PATHS ${AEC_PATH} ENV AEC_PATH
            PATH_SUFFIXES lib64 lib NO_DEFAULT_PATH)

find_library( AEC_LIBRARY aec PATH_SUFFIXES lib64 lib )

set( AEC_LIBRARIES    ${AEC_LIBRARY} )
set( AEC_INCLUDE_DIRS ${AEC_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)

# handle the QUIET and REQUIRED arguments and set AEC_FOUND to TRUE
# if all listed variables are TRUE
# Note: capitalisation of the package name must be the same as in the file name
find_package_handle_standard_args(AEC DEFAULT_MSG AEC_LIBRARY AEC_INCLUDE_DIR)

mark_as_advanced(AEC_INCLUDE_DIR AEC_LIBRARY)
