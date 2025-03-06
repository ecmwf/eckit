# (C) Copyright 2025- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


import pytest

GRIDSPECS = [
    (dict(grid="1/1"), (360, 181)),
    (dict(grid="H2", ordering="nested"), (12 * 2 * 2)),
    (dict(grid=[2, 2]), (180, 91)),
    ("{grid: 3/3}", (120, 61)),
    (dict(grid="o2"), (sum([20, 24, 24, 20]))),
]


@pytest.mark.parametrize( "_spec, _shape", GRIDSPECS)
def test_gridspec(tmp_path, _spec, _shape):
    from eckit.geo import Grid

    grid = Grid(_spec)
    assert _shape == grid.shape
