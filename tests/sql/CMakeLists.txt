# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


set (_sql_tests
    select
    simple_functions
)

foreach( _tst ${_sql_tests} )
    ecbuild_add_test( TARGET   eckit_test_sql_${_tst}
                      SOURCES  test_${_tst}.cc
                      LIBS     eckit_sql )
endforeach()
