include(CTest)

foreach(name
        param
        projection
        projection_ll_to_xyz
        projection_rotation
        types
        util)
    add_executable(test_${name} ${name}.cc)
    target_link_libraries(test_${name} grit)
    add_test(test_${name} test_${name})
endforeach()

if(PROJ_FOUND)
    add_executable(test_projection_proj projection_proj.cc)
    target_link_libraries(test_projection_proj grit)
    add_test(test_projection_proj test_projection_proj)
endif()

if(eckit_FOUND)
    add_executable(test_search search.cc)
    target_link_libraries(test_search grit eckit)
    add_test(test_search test_search)
endif()

