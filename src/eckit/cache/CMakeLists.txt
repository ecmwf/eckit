# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


list(APPEND eckit_cache_srcs
    DiskCache.cc
    DiskCache.h
    Exceptions.cc
    Exceptions.h
    FileCache.cc
    FileCache.h
    LibEcKitCache.cc
    LibEcKitCache.h
    Loader.cc
    Loader.h
    MemoryUsage.cc
    MemoryUsage.h
    PathLock.cc
    PathLock.h
    Roots.cc
    Roots.h
    Shmget.cc
    Shmget.h
    loader/File.cc
    loader/File.h
    loader/Stream.cc
    loader/Stream.h
    loader/MappedAnonymousMemory.cc
    loader/MappedAnonymousMemory.h
    loader/MappedFile.cc
    loader/MappedFile.h
    loader/Memory.cc
    loader/Memory.h
    loader/SharedMemory.cc
    loader/SharedMemory.h
    mutex.h
)

configure_file(eckit_cache_config.h.in eckit_cache_config.h @ONLY)

list(APPEND eckit_cache_srcs ${CMAKE_CURRENT_BINARY_DIR}/eckit_cache_config.h)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/eckit_cache_config.h
    DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/cache
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
