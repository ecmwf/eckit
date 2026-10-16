# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

ecbuild_add_test( TARGET      eckit_test_exceptions
                  SOURCES     test_exceptions.cc
                  LIBS        eckit )

ecbuild_add_test( TARGET      eckit_test_firstfailure
                  SOURCES     test_firstfailure.cc
                  LIBS        eckit )
