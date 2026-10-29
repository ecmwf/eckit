# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

ecbuild_add_test(
    TARGET  eckit_test_blockfile
    SOURCES test_blockfile.cc
    LIBS    eckit_blockfile
)

# golden.ekbf is written by make_golden_blockfile.py (independent of the C++ implementation)
file( COPY golden.ekbf DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

ecbuild_add_test(
    TARGET  eckit_test_blockfile_golden
    SOURCES test_blockfile_golden.cc
    LIBS    eckit_blockfile
)

if( TARGET eckit_blockfile_tool )
    ecbuild_add_test(
        TARGET            eckit_test_blockfile_tool
        COMMAND           ${CMAKE_CURRENT_SOURCE_DIR}/test_blockfile_tool.sh
        ARGS              $<TARGET_FILE:eckit_blockfile_tool> ${CMAKE_CURRENT_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()
