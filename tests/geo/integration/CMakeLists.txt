# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

foreach(_test
        multio )
    ecbuild_add_test(
        TARGET      eckit_test_geo_integration_${_test}
        SOURCES     ${_test}.cc
        LIBS        eckit_geo
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${_cache_path}" )
endforeach()
