# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# Cubed sphere (type cubed_sphere) and LFRic (Met Office) cubed sphere: cell centres (CS-LFR-<n>), nodes
# (CS-LFR-<n>-N) and edge midpoints (CS-LFR-<n>-E), (lon, lat) in degrees

import pytest

from eckit.geo import BoundingBox
from eckit.geo import Grid


@pytest.mark.parametrize("n", [1, 2, 48])
def test_shape(n):
    cells = Grid(grid=f"CS-LFR-{n}")
    assert cells.shape == (6, n, n)  # panel, row, column
    assert len(cells) == 6 * n * n
    assert cells.arrangement == "C"
    assert cells.name == f"CS-LFR-{n}"

    nodes = Grid(grid=f"CS-LFR-{n}-N")
    assert nodes.shape == (6 * n * n + 2,)
    assert nodes.arrangement == "N"
    assert nodes.name == f"CS-LFR-{n}-N"

    edges = Grid(grid=f"CS-LFR-{n}-E")
    assert edges.shape == (12 * n * n,)
    assert edges.arrangement == "E"
    assert edges.name == f"CS-LFR-{n}-E"


@pytest.mark.parametrize("arrangement", ["C", "N", "E"])
@pytest.mark.parametrize(
    "lfric, cubed_sphere",
    [
        (dict(), dict()),
        (dict(equatorial_latitude=30), dict(stretching_factor=(1 - 0.5) ** 0.5 / (1 + 0.5) ** 0.5)),
        (
            dict(target_north_pole=[-170, 70]),
            dict(projection=dict(type="rotation", south_pole=[10, -70])),
        ),
    ],
)
def test_lfric_as_cubed_sphere(arrangement, lfric, cubed_sphere):
    # an LFRic grid by name, as a cubed sphere of panels lfric (the same points)
    suffix = "" if arrangement == "C" else f"-{arrangement}"
    a = Grid(grid=f"CS-LFR-4{suffix}", **lfric)
    b = Grid(type="cubed_sphere", n=4, arrangement=arrangement, panels="lfric", **cubed_sphere)

    assert a.type == "cubed_sphere_lfric"
    assert b.type == "cubed_sphere"
    assert a.shape == b.shape
    for x, y in zip(a.to_latlons(), b.to_latlons()):
        assert x == pytest.approx(y, abs=1e-12)

    assert Grid(b.spec) == b


@pytest.mark.parametrize(
    "spec, same",
    [
        (dict(grid="CS-LFR-4"), dict(type="cubed_sphere_lfric", n=4)),
        (dict(grid="CS-LFR-4-N"), dict(type="cubed_sphere_lfric", n=4, arrangement="N")),
        (dict(grid="CS-LFR-4-N"), "{grid: cs_lfr_4_n}"),
        (dict(grid="CS-LFR-4"), "{grid: cs_lfr_4_c}"),
        (dict(grid="CS-LFR-4"), "{grid: Cs-Lfr-4}"),
        (
            dict(grid="CS-LFR-4", target_null_island=[10, 20]),
            dict(grid="CS-LFR-4", target_north_pole=[-170, 70]),
        ),
        (
            dict(grid="CS-LFR-4", target_north_pole=[-170, 70]),
            dict(grid="CS-LFR-4", target_south_pole=[10, -70]),
        ),
    ],
)
def test_spec(spec, same):
    grid = Grid(spec)
    assert grid == Grid(same)
    assert grid == Grid(grid.spec)
    assert grid.type == "cubed_sphere_lfric"


def test_lfric_c48():
    # as LFRic's cubedsphere_mesh_generator: panel 1 first, from its NW corner eastwards
    lat, lon = Grid(grid="CS-LFR-48").to_latlons()
    assert lon[0] == pytest.approx(315.93266075154253, abs=1e-5)
    assert lat[0] == pytest.approx(34.818741563821646, abs=1e-5)
    assert lon[1] == pytest.approx(317.80777901502904, abs=1e-5)
    assert lat[1] == pytest.approx(35.646161623112214, abs=1e-5)

    lat, lon = Grid(grid="CS-LFR-48-N").to_latlons()
    assert (lon[0], lat[0]) == pytest.approx((315, 35.264389682754654), abs=1e-5)


def test_panels():
    np = pytest.importorskip("numpy")

    n = 6
    grid = Grid(grid=f"CS-LFR-{n}")
    lat, lon = (np.array(c).reshape(grid.shape) for c in grid.to_latlons())

    # panels 1 to 4 around the equator, panels 5 and 6 on the poles
    assert all(abs(lat[p]).max() < 46 for p in range(4))
    assert lat[4].min() > 35 and lat[5].max() < -35

    # rows (southwards) of cells (eastwards), on panels 1 to 4
    assert (np.diff(lat[0], axis=0) < 0).all()


def test_stretching_and_rotation():
    n = 4
    centre = (n // 2) * n + n // 2  # panel 1 centre node

    lat, lon = Grid(grid=f"CS-LFR-{n}-N", equatorial_latitude=30).to_latlons()
    assert (lon[centre], lat[centre]) == pytest.approx((0, 30), abs=1e-12)

    lat, lon = Grid(grid=f"CS-LFR-{n}-N", target_null_island=[10, 20]).to_latlons()
    assert (lon[centre], lat[centre]) == pytest.approx((10, 20), abs=1e-12)


def test_search():
    grid = Grid(grid="CS-LFR-48")
    lat, lon = grid.to_latlons()

    index, distance = grid.search().search_nn((lon[1234], lat[1234]))
    assert index == 1234
    assert distance == pytest.approx(0, abs=1e-3)


def test_crop():
    grid = Grid(grid="CS-LFR-12")
    area = BoundingBox(60, -10, 30, 20)  # north, west, south, east
    indices = grid.crop(area)

    lat, lon = grid.to_latlons()
    assert indices
    assert all(30 <= lat[i] <= 60 for i in indices)
    assert len(grid.cropped(area)) == len(indices)


@pytest.mark.parametrize(
    "spec",
    [
        dict(type="cubed_sphere_lfric", n=0),
        dict(type="cubed_sphere_lfric", n=2, arrangement="L"),
        dict(type="cubed_sphere_lfric", n=2, equatorial_latitude=90),
        dict(grid="CS-LFR-L-2"),
        dict(type="cubed_sphere", n=2),  # panels expected
        dict(type="cubed_sphere", n=2, panels="unknown"),
    ],
)
def test_errors(spec):
    with pytest.raises(Exception):
        Grid(spec)
