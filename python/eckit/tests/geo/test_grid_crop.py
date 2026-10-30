# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


import pytest
from eckit.geo import Area
from eckit.geo import BoundingBox
from eckit.geo import Grid

# two disjoint rings, off the 1-degree grid points: 10 x 10 and 10 x 9 points inside
POLYGONS = dict(
    type="polygon",
    rings=[
        dict(latitudes=[0.5, 0.5, 10.5, 10.5], longitudes=[0.5, 10.5, 10.5, 0.5]),
        dict(latitudes=[-19.5, -19.5, -10.5, -10.5], longitudes=[100.5, 110.5, 110.5, 100.5]),
    ],
)


def same_points(cropped, grid, indices) -> bool:
    """The cropped grid points (lon, lat) are those of the grid, by index"""
    lat, lon = grid.to_latlons()
    cropped_lat, cropped_lon = cropped.to_latlons()
    return cropped_lat == pytest.approx([lat[i] for i in indices]) and [
        (x - lon[i] + 180) % 360 - 180 for x, i in zip(cropped_lon, indices)
    ] == pytest.approx([0] * len(indices))


@pytest.mark.parametrize("spec", [dict(grid=[1, 1]), dict(grid="O32")])
@pytest.mark.parametrize(
    "area",
    [
        BoundingBox(10, -10, -10, 10),
        dict(type="bounding_box", north=60.5, west=349.5, south=40.5, east=30.5),
    ],
)
def test_unprojected_grid_crop(spec, area):
    grid = Grid(spec)
    indices = grid.crop(area)
    assert indices

    # the cropped grid has the same points, in the same order
    cropped = grid.cropped(area)
    assert cropped.size() == len(indices)

    lat, lon = grid.to_latlons()
    cropped_lat, cropped_lon = cropped.to_latlons()
    assert cropped_lat == pytest.approx([lat[i] for i in indices])
    assert [(x - lon[i] + 180) % 360 - 180 for x, i in zip(cropped_lon, indices)] == pytest.approx([0] * len(indices))


def test_polygons():
    grid = Grid(grid=[1, 1])
    assert len(grid.crop(POLYGONS)) == 10 * 10 + 10 * 9
    assert len(grid.crop(Area(POLYGONS))) == 10 * 10 + 10 * 9


@pytest.mark.parametrize(
    "spec, area",
    [
        (dict(grid=[1, 1], rotation=[-40, 10]), BoundingBox(10, -10, -10, 10)),
        (
            dict(
                x=[0, 120000, 2500],
                y=[0, 170000, 2500],
                false=[60000.021, 85000.055],
                projection=dict(type="lcc", lat_0=51.967, lat_1=51.967, lon_0=4.9, reverse=True),
            ),
            BoundingBox(52, 4.5, 51.5, 5.5),
        ),
    ],
)
def test_projected_grid_crop(spec, area):
    grid = Grid(spec)

    # points in the area (geographic), as the grid
    indices = grid.crop(area)
    assert indices and indices == sorted(indices)

    lat, lon = grid.to_latlons()
    for i in indices:
        assert area.contains_point(lon[i], lat[i])

    # projected grids crop as unstructured grids, of the same points
    cropped = grid.cropped(area)
    assert cropped.type == "unstructured_ll"
    assert same_points(cropped, grid, indices)


LCC = dict(
    x=[0, 120000, 2500],
    y=[0, 170000, 2500],
    false=[60000.021, 85000.055],
    projection=dict(type="lcc", lat_0=51.967, lat_1=51.967, lon_0=4.9, reverse=True),
)


@pytest.mark.parametrize(
    "spec, area, type",
    [
        (dict(grid=[1, 1]), BoundingBox(10, -10, -10, 10), "regular_ll"),
        (dict(grid="F32"), BoundingBox(10, -10, -10, 10), "regular_gg"),
        (dict(grid="O32"), BoundingBox(10, -10, -10, 10), "reduced_gg"),
        (dict(grid=[1, 1]), POLYGONS, "unstructured_ll"),
        (dict(grid=[1, 1], rotation=[-40, 10]), BoundingBox(10, -10, -10, 10), "unstructured_ll"),
        (LCC, BoundingBox(52, 4.5, 51.5, 5.5), "unstructured_ll"),
    ],
)
def test_cropped(spec, area, type):
    grid = Grid(spec)
    indices = grid.crop(area)

    # of the same type if supported, otherwise unstructured
    cropped = grid.cropped(area)
    assert cropped.type == type
    assert same_points(cropped, grid, indices)

    # as unstructured, always: its coordinates cached, exchangeable by uid
    unstructured = grid.cropped_unstructured_ll(area)
    assert unstructured.type == "unstructured_ll"
    assert same_points(Grid(uid=unstructured.uid), grid, indices)


def test_cache_is_temporary():
    import os

    from eckit.geo import cache

    # tests do not write to the runtime cache (see conftest)
    writable = cache()["paths"]["writable"]
    assert os.path.realpath(writable[0]) == os.path.realpath(os.environ["ECKIT_GEO_CACHE_PATH"])


@pytest.mark.parametrize("spec, type", [(dict(grid=[9, 9]), "regular_ll"), (dict(grid="O2"), "reduced_gg")])
def test_global_grid_by_global_bounding_box_from_another_longitude(spec, type):
    # all points, rows from longitude -180 (180), the grids from longitude 0
    grid = Grid(spec)
    area = BoundingBox(90, -180, -90, 360)

    indices = grid.crop(area)
    assert sorted(indices) == list(range(grid.size()))
    assert indices != sorted(indices)

    lat, lon = grid.to_latlons()
    first = [i for n, i in enumerate(indices) if n == 0 or lat[i] != lat[indices[n - 1]]]
    assert all((lon[i] + 180) % 360 == pytest.approx(0) for i in first)

    # the cropped grid, of the same type, has the same points in the same order
    cropped = grid.cropped(area)
    assert cropped.type == type
    assert same_points(cropped, grid, indices)
