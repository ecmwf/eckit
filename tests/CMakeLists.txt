include(CTest)

foreach(name
        projection
        projection_ll_to_xyz
        projection_rotation
        types
        util)
    add_executable(test_${name} ${name}.cc)
    target_link_libraries(test_${name} grit)
    add_test(test_${name} test_${name})
endforeach()

