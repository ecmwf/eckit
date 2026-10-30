# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


from json import loads

import pytest

from eckit.geo import Figure
from eckit.geo import Grid
from eckit.geo import Projection
from eckit.geo import cache
from eckit.geo import cache_purge

SPECS = [
    (dict(grid="1/1"), (181, 360)),
    (dict(grid="H2", ordering="nested"), (12 * 2 * 2,)),
    (dict(grid=[2, 2]), (91, 180)),
    ("{grid: 3/3}", (61, 120)),
    (dict(grid="o2"), (sum([20, 24, 24, 20]),)),
]


@pytest.mark.parametrize("spec, shape", SPECS)
def test_grid_shape(spec, shape):
    grid = Grid(spec)
    assert shape == grid.shape


def test_grid_unstructured_ll():
    grid = Grid(latitudes=[1, 2, 3], longitudes=[4, 5, 6])

    assert grid.spec == dict(type="unstructured_ll", uid=grid.uid)
    assert grid == Grid(uid=grid.uid)

    name = "another-custom-grid"
    grid = Grid(longitudes=[4, 5, 6], latitudes=[1, 2, 3], name=name)

    assert grid == Grid(name)
    assert grid.spec == dict(grid=name)


def test_grid_from_uid():
    grid = Grid({"grid": [0.5, 0.5]})
    assert Grid({"uid": grid.uid}) == grid


def test_grid_unstructured_ll_from_coordinates():
    from eckit.geo._eckit_geo import _spec_str

    lat, lon = Grid(grid="H4", order="nested").to_latlons()
    grid = Grid(latitudes=lat, longitudes=lon)

    assert grid == Grid(_spec_str(dict(latitudes=lat, longitudes=lon)))
    assert grid == Grid(uid=grid.uid)
    assert grid.shape == (len(lat),)
    assert grid.to_latlons() == (lat, lon)


def test_grid_to_unstructured_ll():
    grid = Grid(latitudes=[1, 2, 3], longitudes=[4, 5, 6])

    a = grid.to_unstructured_ll()
    assert a.type == "unstructured_ll"
    assert a.to_latlons() == grid.to_latlons()
    assert a.uid == grid.uid  # no name given, same points -> same uid

    name = "custom-to-unstructured-ll"
    b = grid.to_unstructured_ll(name)
    assert b.uid == grid.uid  # name doesn't change uid
    assert b == Grid(name)


def test_grid_regular_ll():
    spec = dict(
        area=[71.9834273, -25, 25, 49.9834833],
        grid=[0.0166667, 0.0166667],
        reference=[5e-05, 0.0166167],
    )
    grid = Grid(spec)
    assert grid.spec == spec


def test_grid_rotated():
    canonical = dict(
        grid=[5, 5],
        projection=dict(south_pole=[-20, -40], type="rotation"),
    )

    for spec in [
        dict(grid=[5, 5], rotation=[-40, -20]),
        canonical,
        dict(grid=[5, 5], projection=dict(south_pole_lon=-20, south_pole_lat=-40, type="rotation")),
        dict(grid=[5, 5], projection=dict(rotation=[-40, -20], type="rotation")),
    ]:
        assert Grid(spec).spec == canonical


def test_grid_rotated_default_south_pole_is_not_a_rotation():
    for spec in [
        dict(grid=[5, 5]),
        dict(grid=[5, 5], rotation=[-90, 0]),
        dict(grid=[5, 5], projection=dict(south_pole=[0, -90], type="rotation")),
    ]:
        assert Grid(spec).spec == dict(grid=[5, 5])


def test_grid_rotated_south_pole_lon_lat_optional():
    for rotation, projection in [
        ([-90, -20], dict(south_pole_lon=-20, type="rotation")),
        ([-40, 0], dict(south_pole_lat=-40, type="rotation")),
    ]:
        assert Grid(dict(grid=[5, 5], projection=projection)).spec == Grid(dict(grid=[5, 5], rotation=rotation)).spec


def test_grid_rotated_gg():
    grid = Grid(grid="F48", rotation=[30, 30])
    assert grid.spec == dict(grid="F48", projection=dict(south_pole=[30, 30], type="rotation"))

    lats, lons = grid.to_latlons()
    assert list(lats[:3]) == pytest.approx([-28.57216851400726, -28.57292230625801, -28.57518290821670])
    assert list(lons[:3]) == pytest.approx([-150.00000000000000, -150.05319064425672, -150.10632666115495])


def test_grid_figure():
    grid = Grid(grid="1/1")
    figure = grid.figure

    assert isinstance(figure, Figure)
    assert figure.spec_str == figure.spec_str
    assert figure.spec == loads(figure.spec_str)
    assert figure.a == figure.b  # default figure is spherical
    assert figure.spherical

    # borrowed reference stays valid once the grid goes out of scope
    del grid
    assert figure.R > 0


def test_grid_projection():
    grid = Grid(grid="1/1")
    projection = grid.projection

    assert isinstance(projection, Projection)
    assert projection.spec == loads(projection.spec_str)
    assert projection.type

    assert isinstance(projection.figure, Figure)
    assert projection.figure == grid.figure

    del grid
    assert projection.spec_str


@pytest.mark.parametrize(
    "spec, expected",
    [
        (dict(grid=[30, 30]), dict(grid=[30, 30])),
        (dict(grid=[30, 30], figure="earth"), dict(grid=[30, 30])),
        (dict(grid=[30, 30], figure="wgs84"), dict(grid=[30, 30], figure="wgs84")),
        (dict(grid="F48", rotation=[30, 30]), dict(grid="F48", projection=dict(type="rotation", south_pole=[30, 30]))),
    ],
)
def test_grid_spec(spec, expected):
    # the area, projection and figure are part of the grid spec, unless the defaults
    grid = Grid(spec)
    assert grid.spec == expected
    assert Grid(grid.spec) == grid


@pytest.mark.parametrize(
    "spec, a, b",
    [
        (dict(grid=[30, 30]), 6371229.0, 6371229.0),  # default figure
        (dict(grid=[30, 30], figure="wgs84"), 6378137.0, 6356752.314245),
    ],
)
def test_grid_to_xyz(spec, a, b):
    grid = Grid(spec)
    lat, _ = grid.to_latlons()
    x, y, z = grid.to_xyz()
    assert len(x) == len(y) == len(z) == grid.size()

    # radius at the equator and the poles
    for i, la in enumerate(lat):
        radius = (x[i] ** 2 + y[i] ** 2 + z[i] ** 2) ** 0.5
        if la == 0:
            assert radius == pytest.approx(a)
        if abs(la) == 90:
            assert radius == pytest.approx(b)


@pytest.mark.parametrize(
    "spec, coordinates",
    [
        (dict(grid=[30, 30]), ("longitude", "latitude")),
        (dict(grid=[30, 30], rotation=[-40, -20]), ("longitude", "latitude")),
        (dict(grid="o2"), ("longitude", "latitude")),
        (dict(x=[0, 2, 1], y=[0, 1, 1]), ("x", "y")),
    ],
)
def test_grid_point_coordinates(spec, coordinates):
    grid = Grid(spec)
    assert grid.point_coordinates == coordinates
    assert tuple(grid.to_points()) == coordinates


def indices(neighbours):
    return [index for index, _ in neighbours]


def test_grid_search():
    grid = Grid(grid=[30, 30])
    search = grid.search()
    assert len(search) == len(grid)

    # rows of 12 points from the North Pole: (lon, lat) = (0, 0) is point 36
    index, distance = search.search_nn((0, 0))
    assert index == 36
    assert distance == pytest.approx(0, abs=1e-6)

    # equidistant to (0, 0) and (30, 0), in metres (chord on the Earth)
    knn = search.search_knn((15, 0), 2)
    assert sorted(indices(knn)) == [36, 37]
    assert knn[0][1] == pytest.approx(knn[1][1])
    assert knn[0][1] == pytest.approx(1.663e6, rel=1e-3)

    radius = knn[0][1] * 1.001
    assert sorted(indices(search.search_radius((15, 0), radius))) == [36, 37]
    assert sorted(indices(search.search_knn_and_radius((15, 0), 4, radius))) == [36, 37]
    assert len(search.search_knn_or_radius((15, 0), 4, 1.0)) == 4


def test_grid_search_spec():
    grid = Grid(grid=[30, 30])

    search = grid.search(search="knn", k=3)
    assert search.search((15, 0)) == search.search_knn((15, 0), 3)

    with pytest.raises(RuntimeError):
        grid.search(search="knn")  # k is required


def test_grid_search_footprint():
    grid = Grid(grid=[30, 30])

    cache_purge()

    # the k-d tree loader is the library's (see cache())
    search = grid.search()

    # the cache accounts for the loaded search, in process memory or shared
    searches = cache()["memory"]["search"]
    assert searches["size"] == 1
    assert search.footprint["memory"] + search.footprint["shared"] > 0
    assert searches["footprint"] == search.footprint


@pytest.mark.parametrize(
    "spec, first_last",
    [
        (
            dict(
                x=[0, 44961.0033558134, 2997.40022372089],
                y=[0, -44961.0033558134, -2997.40022372089],
                false=[1067074.48, -467594.435],
                projection=dict(type="space_view", h=35747054.5503, reverse=True),
            ),
            [(4.223693723, 350.299524805), (3.815601110, 350.719698129)],
        ),
        (
            dict(
                x=[0, 45009.7522881123, 3000.65015254082],
                y=[0, -33005.8361442917, -3000.53055857197],
                false=[-1632353.683, -1968348.046],
                projection=dict(type="space_view", h=35785813.2659, lon_0=-75, figure="wgs84", reverse=True),
            ),
            [(18.425011761, 300.905565350), (18.106822780, 301.327135202)],
        ),
    ],
)
def test_grid_space_view(spec, first_last):
    # as GRIB space_view (by eccodes), points as the eccodes iterator
    grid = Grid(spec)
    assert grid.spec == spec
    assert grid.shape in ((16, 16), (12, 16))

    lat, lon = grid.to_latlons()
    for (lat0, lon0), i in zip(first_last, (0, -1)):
        assert lat[i] == pytest.approx(lat0, abs=1e-6)
        assert (lon[i] - lon0 + 180.0) % 360.0 - 180.0 == pytest.approx(0.0, abs=1e-6)
