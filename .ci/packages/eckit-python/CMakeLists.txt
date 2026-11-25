# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# The eckit Python bindings (PyPI: eckit) as a wheel, against an installed eckit:
#   cmake -S .ci/packages/eckit-python -B build -DCMAKE_PREFIX_PATH=<eckit and its deps>
# `cmake --install` writes the wheel to <prefix>/wheels. Without VERSION_SUFFIX
# (set by the PyPI build) it does not require eckitlib.

cmake_minimum_required( VERSION 3.18 FATAL_ERROR )

project( eckit-python LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

find_package( eckit REQUIRED )
find_package( Python3 3.11 REQUIRED COMPONENTS Interpreter Development.Module )

set( _repo "${CMAKE_CURRENT_SOURCE_DIR}/../../.." )
set( _staging "${CMAKE_BINARY_DIR}/wheel-staging" )

file( REMOVE_RECURSE "${_staging}" )
file( COPY "${_repo}/python/eckit/" DESTINATION "${_staging}"
      PATTERN "tests" EXCLUDE
      PATTERN "VERSION" EXCLUDE )
configure_file( "${_repo}/VERSION" "${_staging}/VERSION" COPYONLY )

set( PYTHONEXT_INSTALL_DIR "${_staging}/src" CACHE PATH "Install directory for Python extensions" FORCE )
add_subdirectory( "${_repo}/python" python )

install( CODE "
    execute_process(
        COMMAND \"${Python3_EXECUTABLE}\" -m build --wheel --outdir \"\${CMAKE_INSTALL_PREFIX}/wheels\" \"${_staging}\"
        COMMAND_ERROR_IS_FATAL ANY
    )
" )
