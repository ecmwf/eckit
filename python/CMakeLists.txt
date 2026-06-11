# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

# Require Cython; Install destination: prefix-relative site-packages
execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import cython; import sysconfig; print(sysconfig.get_path('platlib', vars={'platbase': '${CMAKE_INSTALL_PREFIX}', 'base': '${CMAKE_INSTALL_PREFIX}'}))"
    OUTPUT_VARIABLE _python_sitearch
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
)

set(_src ${CMAKE_CURRENT_SOURCE_DIR}/eckit/src/_eckit)
set(PYTHONEXT_INSTALL_DIR ${_python_sitearch} CACHE PATH "Install directory for Python extensions")
set(_cython_flags
    --cplus
    --directive language_level=3
    --directive c_string_type=unicode
    --directive c_string_encoding=utf8
)

# ---- eckit._eckit ----
set(_eckit_cpp ${CMAKE_CURRENT_BINARY_DIR}/_eckit.cpp)

add_custom_command(
    OUTPUT ${_eckit_cpp}
    COMMAND
        ${Python3_EXECUTABLE} -m cython
        ${_cython_flags}
        -I ${_src}
        ${_src}/_eckit.pyx
        -o ${_eckit_cpp}
    DEPENDS
        ${_src}/_eckit.pyx
        ${_src}/eckit.pxd
        ${_src}/std.pxd
    COMMENT "Cythonizing _eckit.pyx"
    VERBATIM
)

Python3_add_library(_eckit MODULE WITH_SOABI ${_eckit_cpp} ${_src}/eckit.cc)

target_include_directories(_eckit PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${_src})
target_link_libraries(_eckit PRIVATE eckit)

# ---- eckit.geo._eckit_geo ----
if(eckit_HAVE_ECKIT_GEO)
    set(_eckit_geo_cpp ${CMAKE_CURRENT_BINARY_DIR}/_eckit_geo.cpp)
    add_custom_command(
        OUTPUT ${_eckit_geo_cpp}
        COMMAND
            ${Python3_EXECUTABLE} -m cython
            ${_cython_flags}
            -I ${_src}
            ${_src}/_eckit_geo.pyx
            -o ${_eckit_geo_cpp}
        DEPENDS
            ${_src}/_eckit_geo.pyx
            ${_src}/eckit_geo.pxd
            ${_src}/std.pxd
        COMMENT "Cythonizing _eckit_geo.pyx"
        VERBATIM
    )

    Python3_add_library(_eckit_geo MODULE WITH_SOABI ${_eckit_geo_cpp} ${_src}/eckit.cc)

    target_include_directories(_eckit_geo PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${_src})
    target_link_libraries(_eckit_geo PRIVATE eckit_geo)

endif()

install(TARGETS _eckit             DESTINATION ${PYTHONEXT_INSTALL_DIR}/eckit     COMPONENT python)
install(TARGETS _eckit_geo         DESTINATION ${PYTHONEXT_INSTALL_DIR}/eckit/geo COMPONENT python)
install(DIRECTORY eckit/src/eckit/ DESTINATION ${PYTHONEXT_INSTALL_DIR}/eckit     COMPONENT python FILES_MATCHING PATTERN "*.py")

