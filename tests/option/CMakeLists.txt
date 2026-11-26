# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

foreach( TESTCASE RANGE 1 40 )
  ecbuild_add_test(
    TARGET      eckit_test_option_cmdargs_${TESTCASE}
    SOURCES     eckit_test_option_cmdargs.cc
    LIBS        eckit_option
    DEFINITIONS TESTCASE=${TESTCASE}
  )
endforeach()

ecbuild_add_test(
    TARGET      eckit_test_option_factory
    SOURCES     eckit_test_option_factory.cc
    LIBS        eckit_option
)
