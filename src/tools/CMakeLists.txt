
foreach(name
        griditer-version)
    add_executable(${name} ${name}.cc)
    target_link_libraries(${name} grit)
endforeach()

