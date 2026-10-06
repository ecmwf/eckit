# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0
#
# Requires:
#  FindPackageHandleStandardArgs (CMake standard module)
#

#[=======================================================================[.rst:
FindLibOPENFAM
--------------

This module finds the libopenfam (Fabric-Attached Memory) library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``LibOPENFAM``
  The libopenfam library

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``LIB_OPENFAM_FOUND``
  True if the libopenfam library is found.
``LIB_OPENFAM_INCLUDE_DIRS``
  Include directories needed to use libopenfam.
``LIB_OPENFAM_LIBRARIES``
  Libraries needed to link to libopenfam.

Cache variables
^^^^^^^^^^^^^^^

The following cache variables may also be set to help find libopenfam library:

``LIB_OPENFAM_INCLUDE_DIR``
  where to find the libopenfam headers.
``LIB_OPENFAM_LIBRARY``
  where to find the libopenfam library.

Hints
^^^^^

The variables ``OPENFAM_ROOT``, ``OPENFAM_DIR`` and ``OPENFAM_PATH``, or the
environment variables of the same names, may also be set to help find libopenfam
library. ``LibOPENFAM_ROOT`` is honoured by CMake itself (CMP0074).

#]=======================================================================]

set(_lib_openfam_hints
    ${OPENFAM_ROOT}
    ${OPENFAM_DIR}
    ${OPENFAM_PATH}
    ENV OPENFAM_ROOT
    ENV OPENFAM_DIR
    ENV OPENFAM_PATH
)

find_path(LIB_OPENFAM_INCLUDE_DIR fam/fam.h
    HINTS ${_lib_openfam_hints}
    PATH_SUFFIXES include
)

find_library(LIB_OPENFAM_LIBRARY
    NAMES openfam
    HINTS ${_lib_openfam_hints}
    PATH_SUFFIXES lib lib64 build/lib build/lib64
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(LibOPENFAM REQUIRED_VARS
                                    LIB_OPENFAM_LIBRARY
                                    LIB_OPENFAM_INCLUDE_DIR)

if (LibOPENFAM_FOUND)
    set(LIB_OPENFAM_FOUND TRUE)
    set(LIB_OPENFAM_LIBRARIES ${LIB_OPENFAM_LIBRARY})
    set(LIB_OPENFAM_INCLUDE_DIRS ${LIB_OPENFAM_INCLUDE_DIR})

    # libopenfam is built without its internal helpers linked in, so these must be added explicitly.
    # They are absent from installs that link them statically, hence the optional handling below.
    set(_lib_openfam_extra_libraries)
    foreach(_lib famresmgr famcommon)
        string(TOUPPER ${_lib} _var)
        find_library(LIB_OPENFAM_${_var}_LIBRARY
            NAMES ${_lib}
            HINTS ${_lib_openfam_hints}
            PATH_SUFFIXES lib lib64 build/lib build/lib64
        )
        if(LIB_OPENFAM_${_var}_LIBRARY)
            list(APPEND _lib_openfam_extra_libraries ${LIB_OPENFAM_${_var}_LIBRARY})
        endif()
        mark_as_advanced(LIB_OPENFAM_${_var}_LIBRARY)
    endforeach()
    list(APPEND LIB_OPENFAM_LIBRARIES ${_lib_openfam_extra_libraries})

    if(NOT TARGET LibOPENFAM)
        add_library(LibOPENFAM UNKNOWN IMPORTED)
        # IMPORTED_LOCATION already contributes libopenfam itself; only the helpers go here.
        set_target_properties(LibOPENFAM PROPERTIES
            IMPORTED_LOCATION "${LIB_OPENFAM_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIB_OPENFAM_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${_lib_openfam_extra_libraries}")
    endif()
    unset(_lib_openfam_extra_libraries)
endif()

unset(_lib_openfam_hints)

mark_as_advanced(LIB_OPENFAM_INCLUDE_DIR LIB_OPENFAM_LIBRARY)
