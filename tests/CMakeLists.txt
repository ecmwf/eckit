include(CTest)

foreach(name
        pl
        util)
    add_executable(test_${name} ${name}.cc)
    target_link_libraries(test_${name} grit)
    add_test(test_${name} test_${name})
endforeach()

