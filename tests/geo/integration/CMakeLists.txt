# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

foreach(_test
        multio )
    ecbuild_add_test(
        TARGET      eckit_test_geo_integration_${_test}
        SOURCES     ${_test}.cc
        LIBS        eckit_geo
        ENVIRONMENT ${eckit_geo_test_environment}
                    "ECKIT_GEO_CACHE_PATH=${CMAKE_CURRENT_BINARY_DIR}/cache/${_test}:${eckit_geo_test_cache_seeds}" )

    if(TEST eckit_test_geo_integration_${_test})
        set_tests_properties(eckit_test_geo_integration_${_test} PROPERTIES FIXTURES_REQUIRED eckit_geo_cache)
    endif()
endforeach()
