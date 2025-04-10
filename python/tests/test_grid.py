# (C) Copyright 2025- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


import pytest

SPECS = [
    (dict(grid="1/1"), (181, 360)),
    (dict(grid="H2", ordering="nested"), (12 * 2 * 2,)),
    (dict(grid=[2, 2]), (91, 180)),
    ("{grid: 3/3}", (61, 120)),
    (dict(grid="o2"), (sum([20, 24, 24, 20]),)),
]


@pytest.mark.parametrize( "spec, shape", SPECS)
def test_grid(spec, shape):
    from eckit.geo import Grid

    grid = Grid(spec)
    assert shape == grid.shape
