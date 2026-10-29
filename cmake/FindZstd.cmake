# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# - Try to find libzstd
# Once done this will define
#
#  ZSTD_FOUND         - found zstd
#  ZSTD_INCLUDE_DIRS  - the zstd include directories
#  ZSTD_LIBRARIES     - the zstd libraries
#
# The following paths will be searched with priority if set in CMake or env
#
#  ZSTD_PATH          - prefix path of the zstd installation
#  ZSTD_ROOT          - Set this variable to the root installation

# Search with priority for ZSTD_PATH if given as CMake or env var

find_path(ZSTD_INCLUDE_DIR zstd.h
          HINTS $ENV{ZSTD_ROOT} ${ZSTD_ROOT}
          PATHS ${ZSTD_PATH} ENV ZSTD_PATH
          PATH_SUFFIXES include NO_DEFAULT_PATH)

find_path(ZSTD_INCLUDE_DIR zstd.h PATH_SUFFIXES include )

# Search with priority for ZSTD_PATH if given as CMake or env var
find_library(ZSTD_LIBRARY zstd
            HINTS $ENV{ZSTD_ROOT} ${ZSTD_ROOT}
            PATHS ${ZSTD_PATH} ENV ZSTD_PATH
            PATH_SUFFIXES lib64 lib NO_DEFAULT_PATH)

find_library( ZSTD_LIBRARY zstd PATH_SUFFIXES lib64 lib )

set( ZSTD_LIBRARIES    ${ZSTD_LIBRARY} )
set( ZSTD_INCLUDE_DIRS ${ZSTD_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)

# handle the QUIET and REQUIRED arguments and set ZSTD_FOUND to TRUE
# if all listed variables are TRUE
# Note: capitalisation of the package name must be the same as in the file name
find_package_handle_standard_args(Zstd DEFAULT_MSG ZSTD_LIBRARY ZSTD_INCLUDE_DIR)

mark_as_advanced(ZSTD_INCLUDE_DIR ZSTD_LIBRARY)
