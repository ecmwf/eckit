# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

ecbuild_add_test(
    TARGET  eckit_test_config_resource
    SOURCES test_resource.cc
    ARGS    -integer 100 -listlong 88,99,11,22
    LIBS    eckit
)

ecbuild_add_test(
    TARGET  eckit_test_config_configuration
    SOURCES test_configuration.cc
    LIBS    eckit
)
