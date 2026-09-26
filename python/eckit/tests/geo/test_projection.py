# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


import numpy as np
import pytest
from eckit.geo import Projection

LL = ("longitude", "latitude")
XY = ("x", "y")
XYZ = ("x", "y", "z")

EPS_LL = 1e-9  # [degree]
EPS_XY = 1e-6  # [m]


def check_point_coordinates(projection, source, target):
    """The projection states its input/output point coordinates, and fwd/inv honour them."""
    assert (projection.source_point_coordinates, projection.target_point_coordinates) == (source, target)

    p = (1.0, 2.0, 3.0)[: len(source)]
    q = projection.fwd(*p)
    assert len(q) == len(target)
    assert len(projection.inv(*q)) == len(source)

    # the same, named
    assert projection.fwd(**dict(zip(source, p))) == pytest.approx(q, nan_ok=True)

    # a point of the wrong number of coordinates is rejected
    with pytest.raises(TypeError):
        projection.fwd(*((0.0,) * (len(source) + 1)))
    with pytest.raises(TypeError):
        projection.inv(*((0.0,) * (len(target) - 1)))


@pytest.mark.parametrize(
    "spec,type_,source,target",
    [
        (dict(type="rotation", south_pole=[10.0, -40.0]), "rotation", LL, LL),
        (dict(rotation=[-40.0, 10.0]), "rotation", LL, LL),
        (dict(type="eqc"), "eqc", LL, XY),
        (dict(type="plate-carree", lat_ts=60.0), "eqc", LL, XY),
        (dict(type="ll-to-xyz", R=1.0), "ll-to-xyz", LL, XYZ),
    ],
)
def test_projection_point_coordinates(spec, type_, source, target):
    projection = Projection(spec)
    assert projection.type == type_
    check_point_coordinates(projection, source, target)


def test_projection_rotation():
    # fwd: rotated (lon, lat) to geographic (lon, lat)
    rotation = Projection(type="rotation", south_pole=[10.0, -40.0])
    assert rotation.spec == dict(type="rotation", south_pole=[10, -40])

    # the rotated south pole is the given south pole, and the rotated equator is 90 degree away from it
    assert rotation.fwd(0.0, -90.0)[1] == pytest.approx(-40.0, abs=EPS_LL)
    assert rotation.inv(10.0, -40.0)[1] == pytest.approx(-90.0, abs=EPS_LL)
    assert rotation.fwd(0.0, 0.0) == pytest.approx((10.0, 50.0), abs=EPS_LL)

    for p in [(0.0, 0.0), (10.0, 20.0), (-30.0, 45.0), (170.0, -60.0)]:
        assert rotation.inv(*rotation.fwd(*p)) == pytest.approx(p, abs=EPS_LL)

    # many points (specialised) match one point at a time
    lon = np.array([0.0, 10.0, -30.0, 170.0, 45.0])
    lat = np.array([0.0, 20.0, 45.0, -60.0, 89.0])
    lon2, lat2 = rotation.fwd(lon, lat)
    for i in range(lon.size):
        assert (lon2[i], lat2[i]) == pytest.approx(rotation.fwd(lon[i], lat[i]), abs=EPS_LL)

    # no rotation (the south pole is the south pole)
    identity = Projection(type="rotation", south_pole=[0.0, -90.0])
    assert identity.spec == {}
    assert identity.fwd(10.0, 20.0) == pytest.approx((10.0, 20.0), abs=EPS_LL)


def test_projection_equidistant_cylindrical():
    eqc = Projection(type="eqc")
    assert eqc.fwd(10.0, 20.0) == pytest.approx((10.0, 20.0))
    assert eqc.inv(10.0, 20.0) == pytest.approx((10.0, 20.0))

    # alias, standard parallel scales x by cos(lat_ts)
    eqc_60 = Projection(type="plate-carree", lat_ts=60.0)
    assert eqc_60.spec == dict(type="eqc", lat_ts=60)
    assert eqc_60.fwd(10.0, 20.0) == pytest.approx((5.0, 20.0))
    assert eqc_60.inv(5.0, 20.0) == pytest.approx((10.0, 20.0))

    # latitude of origin shifts y
    eqc_lat_0 = Projection(type="eqc", lat_0=10.0)
    assert eqc_lat_0.fwd(10.0, 20.0) == pytest.approx((10.0, 10.0))
    assert eqc_lat_0.inv(10.0, 10.0) == pytest.approx((10.0, 20.0))


def test_projection_equality():
    # equality is by spec, however the projection is described
    assert Projection(type="eqc") == Projection(type="plate-carree")
    assert Projection(type="rotation", south_pole=[10.0, -40.0]) == Projection(rotation=[-40.0, 10.0])
    assert Projection(type="eqc", lat_ts=60.0) != Projection(type="eqc")


def test_projection_coordinates_forms():
    # the same point, given in all the supported forms
    eqc = Projection(type="eqc", lat_ts=60.0)

    expected = (5.0, 20.0)
    assert eqc.fwd(10.0, 20.0) == pytest.approx(expected)  # ordered, one per argument
    assert eqc.fwd(np.float32(10.0), np.array(20.0)) == pytest.approx(expected)  # numpy scalars
    assert eqc.fwd(longitude=10.0, latitude=20.0) == pytest.approx(expected)  # named, in any order
    assert eqc.fwd(latitude=20.0, longitude=10.0) == pytest.approx(expected)
    assert eqc.inv(x=5.0, y=20.0) == pytest.approx((10.0, 20.0))

    # scalars give floats
    assert all(isinstance(c, float) for c in eqc.fwd(10.0, 20.0))


def test_projection_arrays():
    eqc = Projection(type="eqc", lat_ts=60.0)

    lon = np.array([0.0, 10.0, 20.0])
    lat = np.array([0.0, 20.0, 40.0])

    x, y = eqc.fwd(lon, lat)
    assert isinstance(x, np.ndarray) and isinstance(y, np.ndarray)
    assert x == pytest.approx([0.0, 5.0, 10.0])
    assert y == pytest.approx(lat)

    lon2, lat2 = eqc.inv(x, y)
    assert lon2 == pytest.approx(lon)
    assert lat2 == pytest.approx(lat)

    # lists, named, non-contiguous and non-float64 arrays (copied) give the same result
    for args, kwargs in (
        (([0.0, 10.0, 20.0], [0.0, 20.0, 40.0]), {}),
        ((), dict(latitude=lat, longitude=lon)),
        ((np.array([0.0, 99.0, 10.0, 99.0, 20.0])[::2], lat), {}),
        ((lon.astype(np.float32), lat.astype(np.int64)), {}),
    ):
        x2, y2 = eqc.fwd(*args, **kwargs)
        assert x2 == pytest.approx(x)
        assert y2 == pytest.approx(y)

    # no points
    x0, y0 = eqc.fwd([], [])
    assert x0.size == 0 and y0.size == 0


def test_projection_arrays_lonlat_to_xyz():
    to_xyz = Projection(type="ll-to-xyz", R=1.0)
    assert (to_xyz.source_point_coordinates, to_xyz.target_point_coordinates) == (LL, XYZ)

    x, y, z = to_xyz.fwd(longitude=[0.0, 90.0, 0.0], latitude=[0.0, 0.0, 90.0])
    assert x == pytest.approx([1.0, 0.0, 0.0], abs=EPS_LL)
    assert y == pytest.approx([0.0, 1.0, 0.0], abs=EPS_LL)
    assert z == pytest.approx([0.0, 0.0, 1.0], abs=EPS_LL)

    lon, lat = to_xyz.inv(x, y, z)
    assert lat == pytest.approx([0.0, 0.0, 90.0], abs=EPS_LL)
    assert lon[:2] == pytest.approx([0.0, 90.0], abs=EPS_LL)  # longitude at the pole is arbitrary


def test_projection_arguments_rejected():
    eqc = Projection(type="eqc")

    with pytest.raises(ValueError):
        eqc.fwd(10.0, [20.0, 30.0])  # scalars and arrays mixed
    with pytest.raises(RuntimeError):
        eqc.fwd([10.0, 20.0], [30.0])  # arrays of different lengths
    with pytest.raises(ValueError):
        eqc.fwd(np.zeros((2, 2)), np.zeros((2, 2)))  # not 1-d
    with pytest.raises(TypeError):
        eqc.fwd(10.0, latitude=20.0)  # ordered and named mixed
    with pytest.raises(TypeError):
        eqc.fwd(x=10.0, y=20.0)  # names of the target (x, y), not of the source (longitude, latitude)
    with pytest.raises(TypeError):
        eqc.fwd(longitude=10.0)  # missing a named coordinate
    with pytest.raises(TypeError):
        eqc.fwd([1.0, 2.0], [3.0, 4.0], [5.0, 6.0])  # (longitude, latitude) are 2 coordinates
    with pytest.raises(TypeError):
        eqc.fwd((10.0, 20.0))  # a point, not ordered coordinates
