
list(APPEND TESTS
    [=[{"grid":"H2","ordering":"nested"}]=]
    [=[{"grid":"H2","ordering":"ring"}]=]
    [=[{"grid":"H4","ordering":"nested"}]=]
    [=[{"grid":"H4","ordering":"ring"}]=]
    [=[{"grid":"H8","ordering":"nested"}]=]
    [=[{"grid":"H8","ordering":"ring"}]=]
    [=[{"grid":"H16","ordering":"nested"}]=]
    [=[{"grid":"H16","ordering":"ring"}]=]
    [=[{"grid":"H32","ordering":"nested"}]=]
    [=[{"grid":"H32","ordering":"ring"}]=]
    [=[{"grid":"H64","ordering":"nested"}]=]
    [=[{"grid":"H64","ordering":"ring"}]=]
    [=[{"grid":"H128","ordering":"nested"}]=]
    [=[{"grid":"H128","ordering":"ring"}]=]
    [=[{"grid":"H256","ordering":"nested"}]=]
    [=[{"grid":"H256","ordering":"ring"}]=]
    [=[{"grid":"H512","ordering":"nested"}]=]
    [=[{"grid":"H512","ordering":"ring"}]=]
    [=[{"grid":"H1024","ordering":"nested"}]=]
    [=[{"grid":"H1024","ordering":"ring"}]=]
    [=[{"grid":"N32"}]=]
    [=[{"grid":"N48"}]=]
    [=[{"grid":"N64"}]=]
    [=[{"grid":"N80"}]=]
    [=[{"grid":"N96"}]=]
    [=[{"grid":"N128"}]=]
    [=[{"grid":"N160"}]=]
    [=[{"grid":"N200"}]=]
    [=[{"grid":"N256"}]=]
    [=[{"grid":"N320"}]=]
    [=[{"grid":"N400"}]=]
    [=[{"grid":"N512"}]=]
    [=[{"grid":"N640"}]=]
    [=[{"grid":"N1024"}]=]
    [=[{"grid":"N1280"}]=]
    [=[{"grid":"O32"}]=]
    [=[{"grid":"O48"}]=]
    [=[{"grid":"O64"}]=]
    [=[{"grid":"O80"}]=]
    [=[{"grid":"O96"}]=]
    [=[{"grid":"O128"}]=]
    [=[{"grid":"O160"}]=]
    [=[{"grid":"O200"}]=]
    [=[{"grid":"O256"}]=]
    [=[{"grid":"O320"}]=]
    [=[{"grid":"O400"}]=]
    [=[{"grid":"O512"}]=]
    [=[{"grid":"O640"}]=]
    [=[{"grid":"O1024"}]=]
    [=[{"grid":"O1280"}]=]
    [=[{"grid":"O2560"}]=]
    [=[{"grid":[0.1,0.1]}]=]
    [=[{"grid":[0.125,0.125]}]=]
    [=[{"grid":[0.15,0.15]}]=]
    [=[{"grid":[0.2,0.2]}]=]
    [=[{"grid":[0.25,0.25]}]=]
    [=[{"grid":[0.3,0.3]}]=]
    [=[{"grid":[0.4,0.4]}]=]
    [=[{"grid":[0.5,0.5]}]=]
    [=[{"grid":[0.6,0.6]}]=]
    [=[{"grid":[0.7,0.7]}]=]
    [=[{"grid":[0.75,0.75]}]=]
    [=[{"grid":[0.8,0.8]}]=]
    [=[{"grid":[0.9,0.9]}]=]
    [=[{"grid":[1,1]}]=]
    [=[{"grid":[1.2,1.2]}]=]
    [=[{"grid":[1.25,1.25]}]=]
    [=[{"grid":[1.4,1.4]}]=]
    [=[{"grid":[1.5,1.5]}]=]
    [=[{"grid":[1.6,1.6]}]=]
    [=[{"grid":[1.8,1.8]}]=]
    [=[{"grid":[10,10]}]=]
    [=[{"grid":[2,2]}]=]
    [=[{"grid":[2.5,2.5]}]=]
    [=[{"grid":[5,5]}]=] )

if(eckit_HAVE_GEO_GRID_ORCA)
    list(APPEND TESTS [=[{"grid":"ORCA2_T"}]=])
endif()

foreach(test ${TESTS})
    set(name "eckit_test_geo_integration_eckit-regrid_gridspec_${test}")
    string(MAKE_C_IDENTIFIER "${name}" name)
    string(REGEX REPLACE "_+" "_" name "${name}")
    string(REGEX REPLACE "_$" "" name "${name}")

    ecbuild_add_test(
        TARGET      ${name}
        COMMAND     eckit_grid_spec
        ARGS        "${test}"
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${TEST_ECKIT_GEO_CACHE_PATH}" )

    string(REPLACE "[" [=[\[]=] regex "${test}")
    string(REPLACE "]" [=[\]]=] regex "${regex}")

    set_tests_properties(${name} PROPERTIES PASS_REGULAR_EXPRESSION "${regex}")
endforeach()

# wip
foreach(test
    eckit_test_geo_integration_eckit_regrid_gridspec_grid_0_7_0_7
    eckit_test_geo_integration_eckit_regrid_gridspec_grid_1_4_1_4
    eckit_test_geo_integration_eckit_regrid_gridspec_grid_1_6_1_6 )
    set_tests_properties(${test} PROPERTIES WILL_FAIL TRUE)
endforeach()

