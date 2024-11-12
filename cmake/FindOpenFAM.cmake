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
FindLibOpenFAM
--------

This module finds the openfam library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``LibOpenFAM``
  The openfam library

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``OpenFAM_FOUND``
  True if the openfam library is found.
``OpenFAM_INCLUDE_DIRS``
  Include directories needed to use openfam.
``OpenFAM_LIBRARIES``
  Libraries needed to link to openfam.

Cache variables
^^^^^^^^^^^^^^^

The following cache variables may also be set to help find openfam library:

``OpenFAM_INCLUDE_DIR``
  where to find the openfam headers.
``OpenFAM_LIBRARY``
  where to find the openfam library.

Hints
^^^^^

The environment variables ``OpenFAM_ROOT``, ``OpenFAM_DIR``, and ``OpenFAM_PATH``
may also be set to help find openfam library.

#]=======================================================================]

find_path(OpenFAM_INCLUDE_DIR fam/fam.h
    HINTS
        ${OpenFAM_ROOT}
        ${OpenFAM_DIR}
        ${OpenFAM_PATH}
        ENV OpenFAM_ROOT
        ENV OpenFAM_DIR
        ENV OpenFAM_PATH
)

find_library(OpenFAM_LIBRARY
    NAMES openfam
    HINTS
        ${OpenFAM_ROOT}
        ${OpenFAM_DIR}
        ${OpenFAM_PATH}
        ENV OpenFAM_ROOT
        ENV OpenFAM_DIR
        ENV OpenFAM_PATH
    PATH_SUFFIXES lib lib64
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(OpenFAM
                                  FOUND_VAR OpenFAM_FOUND
                                  REQUIRED_VARS
                                    OpenFAM_LIBRARY
                                    OpenFAM_INCLUDE_DIR)

if (OpenFAM_FOUND)
    set(OpenFAM_INCLUDE_DIRS ${OpenFAM_INCLUDE_DIR}/fam)
    set(OpenFAM_LIBRARIES ${OpenFAM_LIBRARY})
    if(NOT TARGET OpenFAM)
        add_library(OpenFAM INTERFACE IMPORTED)
        set_target_properties(OpenFAM PROPERTIES
            IMPORTED_LOCATION "${OpenFAM_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${OpenFAM_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${OpenFAM_LIBRARY}")
    endif()
endif()

mark_as_advanced(OpenFAM_INCLUDE_DIR OpenFAM_LIBRARY)
