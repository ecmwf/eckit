foreach(_test
        multio )
    ecbuild_add_test(
        TARGET      eckit_test_geo_integration_${_test}
        SOURCES     ${_test}.cc
        LIBS        eckit_geo
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${_cache_path}" )
endforeach()
