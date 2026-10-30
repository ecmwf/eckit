# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


list(APPEND eckit_cache_srcs
    DiskCache.cc
    DiskCache.h
    Exceptions.cc
    Exceptions.h
    LibEcKitCache.cc
    LibEcKitCache.h
    PathLock.cc
    PathLock.h
    Roots.cc
    Roots.h
    mutex.h
)

ecbuild_add_library(
    TARGET eckit_cache
    TYPE SHARED
    INSTALL_HEADERS ALL
    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/cache
    PUBLIC_LIBS eckit

    PUBLIC_INCLUDES
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>

    SOURCES ${eckit_cache_srcs}
)
