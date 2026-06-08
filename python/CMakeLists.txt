# (C) Copyright 2026- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

find_program(CYTHON_EXECUTABLE NAMES cython cython3
    HINTS ${Python3_ROOT_DIR}/bin
    DOC "Cython transpiler"
    REQUIRED)

message(STATUS "eckit Python bindings: Cython=${CYTHON_EXECUTABLE}")

set(_src ${CMAKE_CURRENT_SOURCE_DIR}/eckit/src/_eckit)

set(_cython_flags
    --cplus
    --directive language_level=3
    --directive c_string_type=unicode
    --directive c_string_encoding=utf8
)

# Install destination: prefix-relative site-packages
execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_path('platlib', vars={'platbase': '${CMAKE_INSTALL_PREFIX}', 'base': '${CMAKE_INSTALL_PREFIX}'}))"
    OUTPUT_VARIABLE _python_sitearch
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(ECKIT_PYTHON_INSTALL_DIR ${_python_sitearch} CACHE PATH
    "Install directory for eckit Python extensions")

# ---- eckit._eckit ----
set(_eckit_cpp ${CMAKE_CURRENT_BINARY_DIR}/_eckit.cpp)
add_custom_command(
    OUTPUT ${_eckit_cpp}
    COMMAND ${CYTHON_EXECUTABLE} ${_cython_flags}
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

Python3_add_library(_eckit MODULE WITH_SOABI
    ${_eckit_cpp}
    ${_src}/eckit.cc
)

target_include_directories(_eckit PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${_src})
target_link_libraries(_eckit PRIVATE eckit)
target_compile_features(_eckit PRIVATE cxx_std_17)

install(TARGETS _eckit DESTINATION ${ECKIT_PYTHON_INSTALL_DIR}/eckit COMPONENT python)
install(FILES eckit/src/eckit/__init__.py DESTINATION ${ECKIT_PYTHON_INSTALL_DIR}/eckit COMPONENT python)

# ---- eckit.geo._eckit_geo (only when ECKIT_GEO is enabled) ----
if(eckit_HAVE_ECKIT_GEO)
    set(_eckit_geo_cpp ${CMAKE_CURRENT_BINARY_DIR}/_eckit_geo.cpp)
    add_custom_command(
        OUTPUT ${_eckit_geo_cpp}
        COMMAND ${CYTHON_EXECUTABLE} ${_cython_flags}
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

    Python3_add_library(_eckit_geo MODULE WITH_SOABI
        ${_eckit_geo_cpp}
        ${_src}/eckit.cc
    )

    target_include_directories(_eckit_geo PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${_src})
    target_link_libraries(_eckit_geo PRIVATE eckit_geo)
    target_compile_features(_eckit_geo PRIVATE cxx_std_17)

    install(TARGETS _eckit_geo DESTINATION ${ECKIT_PYTHON_INSTALL_DIR}/eckit/geo COMPONENT python)
    install(FILES eckit/src/eckit/geo/__init__.py DESTINATION ${ECKIT_PYTHON_INSTALL_DIR}/eckit/geo COMPONENT python)
endif()
