# Copyright 2024- European Centre for Medium-Range Weather Forecasts (ECMWF)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.
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

The environment variables ``OPENFAM_DIR`` and ``OPENFAM_PATH``
may also be set to help find libopenfam library.

#]=======================================================================]

find_path(LIB_OPENFAM_INCLUDE_DIR fam/fam.h
    HINTS
        ${OPENFAM_DIR}
        ${OPENFAM_PATH}
        ENV OPENFAM_DIR
        ENV OPENFAM_PATH
    PATH_SUFFIXES include ../include
)

find_library(LIB_OPENFAM_LIBRARY
    NAMES openfam
    HINTS
        ${OPENFAM_DIR}
        ${OPENFAM_PATH}
        ENV OPENFAM_DIR
        ENV OPENFAM_PATH
    PATH_SUFFIXES lib lib64 build/lib build/lib64
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(LibOPENFAM REQUIRED_VARS
                                    LIB_OPENFAM_LIBRARY
                                    LIB_OPENFAM_INCLUDE_DIR)

if (LibOPENFAM_FOUND)
    set(LIB_OPENFAM_LIBRARIES ${LIB_OPENFAM_LIBRARY})
    set(LIB_OPENFAM_INCLUDE_DIRS ${LIB_OPENFAM_INCLUDE_DIR})
    if(NOT TARGET LibOPENFAM)
        add_library(LibOPENFAM UNKNOWN IMPORTED)
        set_target_properties(LibOPENFAM PROPERTIES
            IMPORTED_LOCATION "${LIB_OPENFAM_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIB_OPENFAM_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${LIB_OPENFAM_LIBRARY}")
    endif()
endif()

mark_as_advanced(LIB_OPENFAM_INCLUDE_DIR LIB_OPENFAM_LIBRARY)
