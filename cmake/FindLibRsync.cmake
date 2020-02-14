# (C) Copyright 2020- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

# Try to find librsync (https://librsync.github.io)

# The following paths will be searched, both in the environment and as CMake variables:
#
#  LIBRSYNC_ROOT
#  LIBRSYNC_DIR
#  LIBRSYNC_PATH

# If found, the rsync target will be created.

find_path(LIBRSYNC_INCLUDE_DIR
    NAMES librsync.h
    HINTS
        ${LIBRSYNC_ROOT}
        ${LIBRSYNC_DIR}
        ${LIBRSYNC_PATH}
        ENV LIBRSYNC_ROOT
        ENV LIBRSYNC_DIR
        ENV LIBRSYNC_PATH
    PATH_SUFFIXES include
)

find_library(LIBRSYNC_LIBRARY
    NAMES rsync
    HINTS
        ${LIBRSYNC_ROOT}
        ${LIBRSYNC_DIR}
        ${LIBRSYNC_PATH}
        ENV LIBRSYNC_ROOT
        ENV LIBRSYNC_DIR
        ENV LIBRSYNC_PATH
    PATH_SUFFIXES lib lib64
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibRsync DEFAULT_MSG LIBRSYNC_LIBRARY LIBRSYNC_INCLUDE_DIR)

mark_as_advanced(LIBRSYNC_INCLUDE_DIR LIBRSYNC_LIBRARY)

if(LibRsync_FOUND)
    add_library(rsync UNKNOWN IMPORTED GLOBAL)
    set_target_properties(rsync PROPERTIES
        IMPORTED_LOCATION ${LIBRSYNC_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${LIBRSYNC_INCLUDE_DIR}
    )
endif()
