
list(APPEND TESTS
    [=[{"grid":"H2","order":"nested"}]=]
    [=[{"grid":"H2"}]=]
    [=[{"grid":"H4","order":"nested"}]=]
    [=[{"grid":"H4"}]=]
    [=[{"grid":"H8","order":"nested"}]=]
    [=[{"grid":"H8"}]=]
    [=[{"grid":"H16","order":"nested"}]=]
    [=[{"grid":"H16"}]=]
    [=[{"grid":"H32","order":"nested"}]=]
    [=[{"grid":"H32"}]=]
    [=[{"grid":"H64","order":"nested"}]=]
    [=[{"grid":"H64"}]=]
    [=[{"grid":"H128","order":"nested"}]=]
    [=[{"grid":"H128"}]=]
    [=[{"grid":"H256","order":"nested"}]=]
    [=[{"grid":"H256"}]=]
    [=[{"grid":"H512","order":"nested"}]=]
    [=[{"grid":"H512"}]=]
    [=[{"grid":"H1024","order":"nested"}]=]
    [=[{"grid":"H1024"}]=]
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
    [=[{"grid":[0.05,0.05]}]=]
    [=[{"grid":[0.1,0.1]}]=]
    [=[{"grid":[0.125,0.125]}]=]
    [=[{"grid":[0.15,0.15]}]=]
    [=[{"grid":[0.2,0.2]}]=]
    [=[{"grid":[0.25,0.25]}]=]
    [=[{"grid":[0.3,0.3]}]=]
    [=[{"grid":[0.35,0.35]}]=]
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
    [=[{"grid":[2,2]}]=]
    [=[{"grid":[2.5,2.5]}]=]
    [=[{"grid":[5,5]}]=]
    [=[{"grid":[10,10]}]=]
    [=[{"grid":[30,30]}]=] )

list(APPEND SHAPES
    [48]
    [48]
    [192]
    [192]
    [768]
    [768]
    [3072]
    [3072]
    [12288]
    [12288]
    [49152]
    [49152]
    [196608]
    [196608]
    [786432]
    [786432]
    [3145728]
    [3145728]
    [12582912]
    [12582912]
    [6114]
    [13280]
    [23112]
    [35718]
    [50662]
    [88838]
    [138346]
    [213988]
    [348528]
    [542080]
    [843490]
    [1373624]
    [2140702]
    [5447118]
    [8505906]
    [5248]
    [10944]
    [18688]
    [28480]
    [40320]
    [70144]
    [108160]
    [167200]
    [271360]
    [421120]
    [654400]
    [1067008]
    [1661440]
    [4231168]
    [6599680]
    [26306560]
    [3601,7200]
    [1801,3600]
    [1441,2880]
    [1201,2400]
    [901,1800]
    [721,1440]
    [601,1200]
    [517,1024]
    [451,900]
    [361,720]
    [301,600]
    [257,515]
    [241,480]
    [225,450]
    [201,400]
    [181,360]
    [151,300]
    [145,288]
    [129,258]
    [121,240]
    [113,225]
    [101,200]
    [91,180]
    [73,144]
    [37,72]
    [19,36]
    [7,12] )

if(eckit_HAVE_GEO_GRID_ORCA)
    # eORCA025_T [1740494] is too large as an integration test
    list(APPEND TESTS [=[{"grid":"ORCA2_T"}]=])
    list(APPEND SHAPES [182,149])
endif()

if(eckit_HAVE_GEO_GRID_FESOM)
    list(APPEND TESTS [=[{"grid":"pi_N"}]=])
    list(APPEND SHAPES [3140])
endif()

foreach(test shape IN ZIP_LISTS TESTS SHAPES)
    set(name "eckit_test_geo_integration_eckit-regrid_${test}")
    string(MAKE_C_IDENTIFIER "${name}" name)
    string(REGEX REPLACE "_+" "_" name "${name}")
    string(REGEX REPLACE "_$" "" name "${name}")

    ecbuild_add_test(
        TARGET      ${name}_gridspec
        COMMAND     eckit_grid_spec
        ARGS        "${test}"
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${CMAKE_CURRENT_SOURCE_DIR}/eckit_geo_cache" )

    string(REPLACE "[" [=[\[]=] regex "${test}")
    string(REPLACE "]" [=[\]]=] regex "${regex}")

    set_tests_properties(${name}_gridspec PROPERTIES PASS_REGULAR_EXPRESSION "${regex}")

    ecbuild_add_test(
        TARGET      ${name}_shape
        COMMAND     eckit_grid_spec
        ARGS        "${test}" --shape
        ENVIRONMENT "ECKIT_GEO_CACHE_PATH=${CMAKE_CURRENT_SOURCE_DIR}/eckit_geo_cache" )

    string(REPLACE "[" [=[\[]=] regex "${shape}")
    string(REPLACE "]" [=[\]]=] regex "${regex}")

    set_tests_properties(${name}_shape PROPERTIES PASS_REGULAR_EXPRESSION "${regex}")
endforeach()

# wip
foreach(test
    eckit_test_geo_integration_eckit_regrid_gridspec_grid_0_7_0_7
    eckit_test_geo_integration_eckit_regrid_gridspec_grid_1_4_1_4
    eckit_test_geo_integration_eckit_regrid_gridspec_grid_1_6_1_6 )
    set_tests_properties(${test} PROPERTIES WILL_FAIL TRUE)
endforeach()
