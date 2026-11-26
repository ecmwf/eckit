# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

foreach( _test coordinate_helpers great_circle kdtree sphere kpoint points polygon )
    ecbuild_add_test( TARGET  eckit_test_geometry_${_test}
                      SOURCES test_${_test}.cc
                      LIBS    eckit_geometry )
endforeach()
