# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

foreach(_test
        disk_cache
        download
        file_cache
        memory_cache
        loader
        policy
        roots
        shmget)
    ecbuild_add_test(
        TARGET  eckit_test_cache_${_test}
        SOURCES ${_test}.cc
        LIBS    eckit_cache )
endforeach()

ecbuild_add_test(
    TARGET    eckit_test_cache_unzip
    SOURCES   unzip.cc
    LIBS      eckit_cache libzip::zip
    ARGS      --unzip "${CMAKE_CURRENT_SOURCE_DIR}/test.zip"
    CONDITION eckit_HAVE_ZIP )
