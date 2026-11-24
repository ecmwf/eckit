# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# Examples of searching grid points, (lon, lat) in degrees and distances in metres


import pytest

from eckit.geo import Grid
from eckit.geo import projdb_is_available


def test_nearest_grid_point():
    grid = Grid(grid=[1, 1])
    lat, lon = grid.to_latlons()

    index, distance = grid.search().search_nn((-0.2, 51.6))

    assert (lon[index], lat[index]) == (0, 52)
    assert distance < 50e3


def test_inverse_distance_weighting():
    grid = Grid(grid=[1, 1])
    lat, _ = grid.to_latlons()
    field = lat  # a field of latitudes

    neighbours = grid.search().search_knn((10.3, 45.4), 4)
    weights = [1 / distance for _, distance in neighbours]
    value = sum(w * field[i] for (i, _), w in zip(neighbours, weights)) / sum(weights)

    assert 45 < value < 46


def test_points_within_radius():
    grid = Grid(grid=[1, 1])

    # (0, 0) and its 4 neighbours, 1 degree (~111 km) away
    assert len(grid.search().search_radius((0, 0), 120e3)) == 5


def test_configured_search():
    grid = Grid(grid=[1, 1])

    # 4 nearest, but only within 120 km
    search = grid.search(search="knn_and_radius", k=4, radius=120e3)
    assert len(search.search((0.5, 0))) == 2


@pytest.mark.skipif(not projdb_is_available(), reason="PROJ database not available")
def test_projected_grid():
    grid = Grid(type="swisslv95", x=[2480000, 2840000, 20000], y=[1080000, 1300000, 20000])

    # Bern, the grid spacing is 20 km
    _, distance = grid.search().search_nn((7.44, 46.95))
    assert distance < 15e3
