list(APPEND TESTS
    test_iterator
    test_param
    test_projection
    test_projection_ll_to_xyz
    test_projection_rotation
    test_types
    test_util)

if(PROJ_FOUND)
    list(APPEND TESTS test_projection_proj)
endif()

if(eckit_FOUND)
    list(APPEND TESTS test_search)
endif()

foreach(test ${TESTS})
    add_executable(${test} ${test}.cc)
    target_link_libraries(${test} grit)
    add_test(${test} ${test})
endforeach()

