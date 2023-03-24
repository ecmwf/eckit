# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(grit VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
endif()


# dependencies (all optional)
find_package(eccodes 2.29)

option(REQUIRE_ECCODES "Require ecCodes")
if(REQUIRE_ECCODES AND NOT eccodes_FOUND)
    message(FATAL_ERROR "eccodes_FOUND=${eccodes_FOUND}")
endif()


# library/resources
add_subdirectory(src)
add_subdirectory(etc)
add_subdirectory(share)

macro(get_all_targets targets dir)
    get_property(subdirs DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirs})
        get_all_targets(${targets} ${subdir})
    endforeach()

    get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    list(APPEND ${targets} ${current_targets})
endmacro()

set(all_targets)
get_all_targets(all_targets ${CMAKE_CURRENT_SOURCE_DIR})

install(
    TARGETS ${all_targets}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
    RUNTIME DESTINATION bin)


# tests
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()


