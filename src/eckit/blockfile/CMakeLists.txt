# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

ecbuild_add_library(
    TARGET eckit_blockfile

    INSTALL_HEADERS    ALL
    HEADER_DESTINATION ${INSTALL_INCLUDE_DIR}/eckit/blockfile
    PUBLIC_LIBS        eckit
    PUBLIC_INCLUDES
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
      $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>
    PRIVATE_INCLUDES   "${ZSTD_INCLUDE_DIRS}"
    PRIVATE_LIBS       "${ZSTD_LIBRARIES}"

    SOURCES
        Filter.cc
        Filter.h
        Format.cc
        Format.h
        Reader.cc
        Reader.h
        RecordReader.cc
        RecordReader.h
        Writer.cc
        Writer.h
)
