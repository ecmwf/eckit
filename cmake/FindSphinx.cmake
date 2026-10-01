# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# Tries to find sphinx-build
include(FindPackageHandleStandardArgs)

find_program(
    SPHINX_EXECUTABLE
    NAMES sphinx-build
    DOC "Path to sphinx-build"
)

find_package_handle_standard_args(
    Sphinx
    "Failed to find sphinx-build"
    SPHINX_EXECUTABLE
)
