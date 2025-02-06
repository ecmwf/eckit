# (C) Copyright 2025- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

# Try to find CURL
# See https://curl.se/
#
# Defines:
#  CURL_FOUND - if library was found
#  CURL_INCLUDE_DIRS - include directories
#  CURL_LIBRARIES - libraries to link against
#  CURL_VERSION_STRING - version
#
# The following paths will be searched if set in CMake or env
#
#  CURL_ROOT
#  CURL_DIR


find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_CURL QUIET libcurl)
endif()

find_path(CURL_INCLUDE_DIRS
    NAMES curl/curl.h
    HINTS ${PC_CURL_INCLUDE_DIRS} ${CURL_ROOT} ${CURL_DIR} ENV CURL_ROOT ENV CURL_DIR ENV CPATH
    PATH_SUFFIXES include)

find_library(CURL_LIBRARIES
    NAMES curl
    HINTS ${PC_CURL_LIBRARY_DIRS} ${CURL_ROOT} ${CURL_DIR} ENV CURL_ROOT ENV CURL_DIR ENV LIBRARY_PATH
    PATH_SUFFIXES lib lib64)

set(CURL_VERSION_STRING "")
if(PC_CURL_VERSION_STRING)
    set(CURL_VERSION_STRING ${PC_CURL_VERSION})
else()
    # Fallback: Extract version from curl/curlver.h
    if(CURL_INCLUDE_DIRS)
        file(STRINGS "${CURL_INCLUDE_DIRS}/curl/curlver.h" CURL_VERSION_CONTENT
             REGEX "^#define[\t ]+LIBCURL_VERSION[\t ]+\"([^\"]+)\"")

        if(CURL_VERSION_CONTENT)
            string(REGEX MATCH "\"([^\"]+)\"" CURL_VERSION_MATCH ${CURL_VERSION_CONTENT})
            string(REPLACE "\"" "" CURL_VERSION_STRING ${CURL_VERSION_MATCH})
        endif()
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CURL DEFAULT_MSG CURL_LIBRARIES CURL_INCLUDE_DIRS CURL_VERSION_STRING)

if(CURL_FOUND)
    set(CURL_LIBRARIES ${CURL_LIBRARIES})
    set(CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIRS})

    if(NOT TARGET CURL::CURL)
        add_library(CURL::CURL UNKNOWN IMPORTED)
        set_target_properties(CURL::CURL PROPERTIES
            IMPORTED_LOCATION "${CURL_LIBRARIES}"
            INTERFACE_INCLUDE_DIRECTORIES "${CURL_INCLUDE_DIRS}")
    endif()
endif()

mark_as_advanced(CURL_INCLUDE_DIRS CURL_LIBRARIES CURL_VERSION_STRING)

