# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

foreach(_test custom layered)
    ecbuild_add_test(
        TARGET  eckit_test_spec_${_test}
        SOURCES ${_test}.cc
        LIBS    eckit_spec )
endforeach()
