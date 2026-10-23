# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

ecbuild_add_test( TARGET  eckit_test_net_netservice_limiter
                  SOURCES test_netservice_limiter.cc
                  LIBS    eckit
)
