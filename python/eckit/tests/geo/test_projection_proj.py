# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


import os
from pathlib import Path

import numpy as np
import pytest
from eckit.geo import Projection
from eckit.geo._eckit_geo import projdb_is_available
from eckit.geo._eckit_geo import projdb_set_search_paths

# Only meaningful when eckit was compiled with PROJ *and* a usable proj.db is
# resolved. projdb_is_available() is otherwise False.
pytestmark = pytest.mark.skipif(
    not projdb_is_available(),
    reason="eckit built without PROJ, or no usable proj.db",
)

BAD_DIR = "/does/not/exist"
BAD_DB = "/does/not/exist/proj.db"

LL = ("longitude", "latitude")
XY = ("x", "y")
XYZ = ("x", "y", "z")

EPS_LL = 1e-7  # [degree] (~1 cm, an inverse datum shift is not exact)
EPS_XY = 1e-3  # [m]


def db_path():
    """Path to the proj.db bundled in the eckitlib wheel, or None."""
    try:
        import eckitlib
    except ImportError:
        return None
    db = Path(eckitlib.__file__).parent / "share" / "proj" / "proj.db"
    return db if db.exists() else None


@pytest.fixture(autouse=True)
def _restore_proj_context():
    """Undo the global PROJ-context / env mutations each test performs."""
    saved = {k: os.environ.get(k) for k in ("PROJ_DATA", "PROJ_LIB")}
    yield
    for k, v in saved.items():
        if v is None:
            os.environ.pop(k, None)
        else:
            os.environ[k] = v

    from eckit._utils import configure_projdb

    # Recreate the context (drops any test database path, re-resolves the
    # defaults from the now-restored environment), then re-apply the bundled
    # fallback if this build relies on it.
    projdb_set_search_paths("", [])
    configure_projdb()


def test_projdb_bad_search_paths():
    projdb_set_search_paths(BAD_DB, [BAD_DIR])
    assert not projdb_is_available()


def test_projdb_bad_env():
    os.environ["PROJ_DATA"] = BAD_DIR
    projdb_set_search_paths("", [])
    assert not projdb_is_available()


@pytest.mark.skipif(db_path() is None, reason="no bundled proj.db")
def test_projdb_fallback():
    db = db_path()

    projdb_set_search_paths(BAD_DB, [BAD_DIR])
    assert not projdb_is_available()

    projdb_set_search_paths(str(db), [str(db.parent)])
    assert projdb_is_available()


# (city, WGS84 (lon, lat) [degree], CH1903+ / LV95 (E, N) [m])
SWISS_CITIES = [
    ("Zurich (Hauptbahnhof)", (8.5403, 47.3779), (2683196.612242693, 1248035.313847182)),
    ("Bern (Bundeshaus)", (7.4441, 46.9465), (2600416.254716625, 1199490.540613972)),
    ("Geneva (Jet d'eau)", (6.1557, 46.2074), (2500986.154176505, 1118138.645411988)),
]


@pytest.mark.parametrize("city,lonlat,lv95", SWISS_CITIES, ids=[c[0] for c in SWISS_CITIES])
def test_proj_epsg_4326_to_2056(city, lonlat, lv95):
    to_lv95 = Projection(dict(type="proj", source="EPSG:4326", target="EPSG:2056"))
    to_lonlat = Projection(dict(type="proj", source="EPSG:2056", target="EPSG:4326"))

    assert (to_lv95.source_point_coordinates, to_lv95.target_point_coordinates) == (LL, XY)
    assert (to_lonlat.source_point_coordinates, to_lonlat.target_point_coordinates) == (XY, LL)

    assert to_lv95.fwd(*lonlat) == pytest.approx(lv95, abs=EPS_XY)
    assert to_lv95.inv(*lv95) == pytest.approx(lonlat, abs=EPS_LL)

    assert to_lonlat.fwd(*lv95) == pytest.approx(lonlat, abs=EPS_LL)
    assert to_lonlat.inv(*lonlat) == pytest.approx(lv95, abs=EPS_XY)

    # coordinates ordered or named
    assert to_lv95.fwd(*lonlat) == to_lv95.fwd(longitude=lonlat[0], latitude=lonlat[1])

    with pytest.raises(TypeError):
        to_lv95.fwd(*lonlat, 0.0)  # a geographic point has 2 coordinates

    with pytest.raises(TypeError):
        to_lv95.fwd(lonlat)  # a point, not ordered coordinates


@pytest.mark.parametrize(
    "source,target,source_coordinates,target_coordinates",
    [
        ("EPSG:4326", "EPSG:4326", LL, LL),
        ("EPSG:4326", "EPSG:3857", LL, XY),
        ("EPSG:3857", "EPSG:4326", XY, LL),
        ("EPSG:4326", "EPSG:4978", LL, XYZ),
    ],
)
def test_proj_point_coordinates(source, target, source_coordinates, target_coordinates):
    projection = Projection(type="proj", source=source, target=target)
    assert projection.type == "proj"
    assert projection.source_point_coordinates == source_coordinates
    assert projection.target_point_coordinates == target_coordinates

    # fwd/inv take and return the stated coordinates, ordered or named
    p = (1.0, 2.0, 3.0)[: len(source_coordinates)]
    q = projection.fwd(*p)
    assert len(q) == len(target_coordinates)
    assert len(projection.inv(*q)) == len(source_coordinates)
    assert projection.fwd(**dict(zip(source_coordinates, p))) == pytest.approx(q)

    with pytest.raises(TypeError):
        projection.fwd(*((0.0,) * (len(source_coordinates) + 1)))


def test_proj_epsg_4326_to_4258():
    # WGS 84 (lon, lat) to ETRS89 (lon, lat), a geographic to geographic projection
    # NOTE: EPSG defines WGS 84 to ETRS89 as a null transformation (accuracy ~1 m)
    to_etrs89 = Projection(type="proj", source="EPSG:4326", target="EPSG:4258")
    to_wgs84 = Projection(type="proj", source="EPSG:4258", target="EPSG:4326")

    assert to_etrs89.spec == dict(type="proj", target="EPSG:4258")  # source/target EPSG:4326 is the default
    assert to_wgs84.spec == dict(type="proj", source="EPSG:4258")

    assert to_etrs89.source_point_coordinates == to_etrs89.target_point_coordinates == LL
    assert to_wgs84.source_point_coordinates == to_wgs84.target_point_coordinates == LL

    # figure is the target's: GRS 1980 (ETRS89) and WGS 84 ellipsoids, same semi-major axis, different flattening
    for projection in (to_etrs89, to_wgs84):
        figure = projection.figure
        assert not figure.spherical
        assert figure.a == pytest.approx(6378137.0, abs=EPS_XY)

    for p in [(0.0, 0.0), (10.0, 20.0), (356.3, 40.4), (24.9, 60.2)]:
        assert to_etrs89.fwd(*p) == pytest.approx(p, abs=EPS_LL)
        assert to_etrs89.inv(*p) == pytest.approx(p, abs=EPS_LL)
        assert to_wgs84.fwd(*to_etrs89.fwd(*p)) == pytest.approx(p, abs=EPS_LL)

    # longitudes are returned in [lon_minimum, lon_minimum + 360), by default [0, 360)
    assert to_etrs89.fwd(-3.7, 40.4) == pytest.approx((356.3, 40.4), abs=EPS_LL)

    to_etrs89_180 = Projection(type="proj", source="EPSG:4326", target="EPSG:4258", lon_minimum=-180.0)
    assert to_etrs89_180.fwd(-3.7, 40.4) == pytest.approx((-3.7, 40.4), abs=EPS_LL)
    assert to_etrs89_180.fwd(190.0, 0.0) == pytest.approx((-170.0, 0.0), abs=EPS_LL)


def test_proj_epsg_4326_to_3857_arrays():
    merc = Projection(type="proj", source="EPSG:4326", target="EPSG:3857")

    # the second point fails to project (invalid latitude), resulting in NaN
    lon = [0.0, 0.0, 10.0]
    lat = [0.0, 91.0, 20.0]
    x, y = merc.fwd(longitude=lon, latitude=lat)
    assert np.isnan(x[1]) and np.isnan(y[1])
    assert [x[0], x[2]] == pytest.approx([0.0, 1113194.9079327357], abs=EPS_XY)
    assert [y[0], y[2]] == pytest.approx([0.0, 2273030.9269876895], abs=EPS_XY)

    # same as one point at a time
    for i in (0, 2):
        assert merc.fwd(lon[i], lat[i]) == pytest.approx((x[i], y[i]))

    lon2, lat2 = merc.inv(x[[0, 2]], y[[0, 2]])
    assert lon2 == pytest.approx([0.0, 10.0], abs=EPS_LL)
    assert lat2 == pytest.approx([0.0, 20.0], abs=EPS_LL)
