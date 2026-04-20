# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

#[=======================================================================[.rst:
eckit_find_or_fetch
-------------------

This module provides a macro to optionally find or fetch external dependencies
using either find_package() or FetchContent.

Usage::

  eckit_find_or_fetch(
      NAME <package_name>
      DESCRIPTION "<description>"
      [FIND_PACKAGE_NAME <name>]        # Package name for find_package (defaults to NAME)
      [FIND_PACKAGE_VERSION <version>]  # Minimum version for find_package
      [GIT_REPOSITORY <url>]            # Git repository URL for FetchContent
      [GIT_TAG <tag>]                   # Git tag/branch/commit for FetchContent
      [TARGET <target_name>]            # CMake target to check/use (defaults to NAME::NAME)
      [FETCH_OPTIONS <opt1> <val1> ...] # Options to set before FetchContent_MakeAvailable
      [CONDITION <condition>]           # Condition that must be true to enable
  )

This creates a cache variable ``<NAME>_SOURCE`` with values OFF, FIND, or FETCH.

Variables set on success:
  - ``eckit_HAVE_<NAME>`` : Set to 1 if the library is available
  - ``<NAME>_LIBRARIES``  : Set to the target name for linking

#]=======================================================================]

include_guard(GLOBAL)

macro(eckit_find_or_fetch)
    set(_options "")
    set(_single_args NAME DESCRIPTION FIND_PACKAGE_NAME FIND_PACKAGE_VERSION GIT_REPOSITORY GIT_TAG TARGET CONDITION)
    set(_multi_args FETCH_OPTIONS)

    cmake_parse_arguments(_arg "${_options}" "${_single_args}" "${_multi_args}" ${ARGN})

    if(NOT _arg_NAME)
        message(FATAL_ERROR "eckit_find_or_fetch: NAME is required")
    endif()

    # Uppercase name for variables
    string(TOUPPER "${_arg_NAME}" _NAME_UPPER)

    # Defaults
    if(NOT _arg_FIND_PACKAGE_NAME)
        set(_arg_FIND_PACKAGE_NAME "${_arg_NAME}")
    endif()

    if(NOT _arg_TARGET)
        set(_arg_TARGET "${_arg_NAME}::${_arg_NAME}")
    endif()

    # Check condition if specified
    set(_condition_met TRUE)
    if(_arg_CONDITION)
        if(NOT ${_arg_CONDITION})
            set(_condition_met FALSE)
        endif()
    endif()

    # Create cache variable
    set(${_NAME_UPPER}_SOURCE "OFF" CACHE STRING "${_arg_DESCRIPTION}: OFF, FIND, FETCH")
    set_property(CACHE ${_NAME_UPPER}_SOURCE PROPERTY STRINGS OFF FIND FETCH)

    if(NOT ${_NAME_UPPER}_SOURCE STREQUAL "OFF" AND _condition_met)

        # Check condition
        if(NOT _condition_met)
            message(FATAL_ERROR "${_arg_NAME}: condition not met (${_arg_CONDITION})")
        endif()

        if(${_NAME_UPPER}_SOURCE STREQUAL "FIND")
            if(_arg_FIND_PACKAGE_VERSION)
                find_package(${_arg_FIND_PACKAGE_NAME} ${_arg_FIND_PACKAGE_VERSION} REQUIRED)
            else()
                find_package(${_arg_FIND_PACKAGE_NAME} REQUIRED)
            endif()

        elseif(${_NAME_UPPER}_SOURCE STREQUAL "FETCH")
            if(NOT _arg_GIT_REPOSITORY)
                message(FATAL_ERROR "eckit_find_or_fetch(${_arg_NAME}): GIT_REPOSITORY required for FETCH")
            endif()
            if(NOT _arg_GIT_TAG)
                message(FATAL_ERROR "eckit_find_or_fetch(${_arg_NAME}): GIT_TAG required for FETCH")
            endif()

            include(FetchContent)
            FetchContent_Declare(
                ${_arg_NAME}
                GIT_REPOSITORY ${_arg_GIT_REPOSITORY}
                GIT_TAG        ${_arg_GIT_TAG}
            )

            # Apply fetch options (in pairs: OPTION_NAME VALUE)
            list(LENGTH _arg_FETCH_OPTIONS _fetch_opts_len)
            math(EXPR _fetch_opts_max "${_fetch_opts_len} - 1")
            set(_i 0)
            while(_i LESS _fetch_opts_len)
                list(GET _arg_FETCH_OPTIONS ${_i} _opt_name)
                math(EXPR _j "${_i} + 1")
                if(_j LESS _fetch_opts_len)
                    list(GET _arg_FETCH_OPTIONS ${_j} _opt_value)
                    set(${_opt_name} ${_opt_value} CACHE BOOL "" FORCE)
                endif()
                math(EXPR _i "${_i} + 2")
            endwhile()

            FetchContent_MakeAvailable(${_arg_NAME})
        endif()

        # Check target exists
        if(TARGET ${_arg_TARGET})
            set(eckit_HAVE_${_NAME_UPPER} 1)
            set(${_NAME_UPPER}_LIBRARIES ${_arg_TARGET})
            ecbuild_info("${_arg_NAME} (${${_NAME_UPPER}_SOURCE}): target ${_arg_TARGET}")
        else()
            message(FATAL_ERROR "${_arg_NAME}: target ${_arg_TARGET} not found after ${${_NAME_UPPER}_SOURCE}")
        endif()

    else()
        set(eckit_HAVE_${_NAME_UPPER} 0)
        if(NOT _condition_met AND NOT ${_NAME_UPPER}_SOURCE STREQUAL "OFF")
            message(WARNING "${_arg_NAME}: condition not met (${_arg_CONDITION}), disabling")
            set(${_NAME_UPPER}_SOURCE "OFF" CACHE STRING "${_arg_DESCRIPTION}: OFF, FIND, FETCH" FORCE)
        endif()
    endif()

endmacro()
