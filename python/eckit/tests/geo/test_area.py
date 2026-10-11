# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


import pytest

from eckit.geo import Area
from eckit.geo import Grid


@pytest.mark.parametrize(
    "spec, type_, expected",
    [
        (
            dict(north=90, west=0, south=-90, east=360),
            "bounding_box",
            dict(area=[90, 0, -90, 360]),
        ),
        (dict(area=[10, 1, 0, 10]), "bounding_box", dict(area=[10, 1, 0, 10])),
        (
            dict(type="bounding_box_xy", bounding_box_xy=[0, 0, 1, 1]),
            "bounding_box_xy",
            dict(type="bounding_box_xy", bounding_box_xy=[0, 0, 1, 1]),
        ),
    ],
)
def test_area_spec(spec, type_, expected):
    area = Area(spec)
    assert area.type == type_
    assert area.spec == expected
    assert Area(area.spec) == area


def test_area_equality():
    # equality is by spec, however the area is described
    assert Area(north=90, west=0, south=-90, east=360) == Area(area=[90, 0, -90, 360])
    assert Area(area=[10, 1, 0, 10]) != Area(area=[10, 0, 0, 10])


def test_polygon():
    lat, lon = [0.5, 0.5, 10.5, 10.5], [0.5, 10.5, 10.5, 0.5]
    ring = Grid(latitudes=lat, longitudes=lon)
    assert ring.type == "unstructured_ll"

    polygon = Area(type="polygon", latitudes=lat, longitudes=lon)
    assert polygon.type == "polygon"
    assert polygon.spec == dict(type="polygon", uid=ring.uid)
    assert Area(polygon.spec) == polygon
    assert Area(type="polygon", uid=ring.uid) == polygon

    with pytest.raises(ValueError):
        Area(type="polygon", latitudes=[0, 0, 10], longitudes=[0, 10])


def test_polygon_rings():
    np = pytest.importorskip("numpy")

    # rings as dicts (of lists or arrays), or grids
    first = dict(latitudes=np.array([0.5, 0.5, 10.5, 10.5]), longitudes=np.array([0.5, 10.5, 10.5, 0.5]))
    second = Grid(latitudes=[-19.5, -19.5, -10.5, -10.5], longitudes=[100.5, 110.5, 110.5, 100.5])

    polygon = Area(type="polygon", rings=[first, second])
    assert polygon.spec == dict(type="polygon", uid=[Grid(first).uid, second.uid])
    assert Area(polygon.spec) == polygon

    grid = Grid(grid=[1, 1])
    assert len(grid.crop(polygon)) == 10 * 10 + 10 * 9

