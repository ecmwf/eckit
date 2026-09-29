# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

### sources

if( HAVE_EXPERIMENTAL )
  add_subdirectory( experimental )
endif()

add_subdirectory( eckit  )

add_subdirectory( tools )

add_subdirectory( sandbox )
