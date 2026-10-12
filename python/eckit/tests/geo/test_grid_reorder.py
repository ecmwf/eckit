# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


import pytest
from eckit.geo import Grid

# x and y increasing, so the grid order is i+j+
SPEC = dict(x=[0, 2, 1], y=[0, 1, 1])

# points in each order, the empty order being the default (i+j-)
ORDERS = [
    ("", [(0, 1), (1, 1), (2, 1), (0, 0), (1, 0), (2, 0)]),
    ("i+j-", [(0, 1), (1, 1), (2, 1), (0, 0), (1, 0), (2, 0)]),
    ("i+j+", [(0, 0), (1, 0), (2, 0), (0, 1), (1, 1), (2, 1)]),
    ("i-j-", [(2, 1), (1, 1), (0, 1), (2, 0), (1, 0), (0, 0)]),
    ("i-j+", [(2, 0), (1, 0), (0, 0), (2, 1), (1, 1), (0, 1)]),
    ("j+i-", [(2, 0), (2, 1), (1, 0), (1, 1), (0, 0), (0, 1)]),
    ("j+i+", [(0, 0), (0, 1), (1, 0), (1, 1), (2, 0), (2, 1)]),
    ("j-i-", [(2, 1), (2, 0), (1, 1), (1, 0), (0, 1), (0, 0)]),
    ("j-i+", [(0, 1), (0, 0), (1, 1), (1, 0), (2, 1), (2, 0)]),
]


def test_regular_xy():
    grid = Grid(SPEC)

    assert grid.type == "regular_xy"
    assert grid.order == "i+j+"
    assert len(grid) == 6

    # the grid points are (x, y)
    assert grid.point_coordinates == ("x", "y")

    # a dict of coordinate name to values
    points = grid.to_points()
    assert list(points) == ["x", "y"]
    assert list(points["x"]) == [0, 1, 2, 0, 1, 2]
    assert list(points["y"]) == [0, 0, 0, 1, 1, 1]


def test_to_points_numpy():
    np = pytest.importorskip("numpy")

    points = Grid(SPEC).to_points()
    assert all(isinstance(v, np.ndarray) and v.dtype == np.float64 and v.shape == (6,) for v in points.values())

    # (lon, lat) grids
    points = Grid(grid=[90, 90]).to_points()
    assert list(points) == ["longitude", "latitude"]
    assert len(points["longitude"]) == len(points["latitude"]) == 3 * 4


@pytest.mark.parametrize("order, expected", ORDERS)
def test_regular_xy_reorder(order, expected):
    grid = Grid(SPEC)
    points = list(zip(*grid.to_points().values()))

    # grid point k is point ren[k] in the order
    ren = grid.reorder(order)
    assert sorted(ren) == list(range(6))
    assert points == [expected[r] for r in ren]


def test_regular_xy_reorder_default():
    grid = Grid(SPEC)
    assert grid.reorder() == grid.reorder("") == grid.reorder("i+j-")


def test_reorder_invalid():
    with pytest.raises(Exception):
        Grid(SPEC).reorder("?")


@pytest.mark.parametrize(
    "order, expected",
    [
        ("i+-j+", [(0, 0), (1, 0), (2, 0), (2, 1), (1, 1), (0, 1)]),
        ("j+-i+", [(0, 0), (0, 1), (1, 1), (1, 0), (2, 0), (2, 1)]),
    ],
)
def test_regular_xy_alternating(order, expected):
    # rows (or columns) scanning in alternate directions, as GRIB alternativeRowScanning
    grid = Grid(dict(SPEC, order=order))
    assert grid.order == order

    points = list(zip(*grid.to_points().values()))
    assert points == expected

    # grid point k is point ren[k] in the order i+j+
    ipos_jpos = [(0, 0), (1, 0), (2, 0), (0, 1), (1, 1), (2, 1)]
    assert points == [ipos_jpos[r] for r in grid.reorder("i+j+")]
