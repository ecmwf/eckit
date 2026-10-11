# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


import math
import os
from pathlib import Path

import pytest
from eckit.geo import Grid
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
    projdb_set_search_paths("", [])
    if projdb_is_available():
        pytest.skip("proj.db resolved without the bundled fallback")

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
    assert math.isnan(x[1]) and math.isnan(y[1])
    assert [x[0], x[2]] == pytest.approx([0.0, 1113194.9079327357], abs=EPS_XY)
    assert [y[0], y[2]] == pytest.approx([0.0, 2273030.9269876895], abs=EPS_XY)

    # same as one point at a time
    for i in (0, 2):
        assert merc.fwd(lon[i], lat[i]) == pytest.approx((x[i], y[i]))

    lon2, lat2 = merc.inv([x[0], x[2]], [y[0], y[2]])
    assert lon2 == pytest.approx([0.0, 10.0], abs=EPS_LL)
    assert lat2 == pytest.approx([0.0, 20.0], abs=EPS_LL)


@pytest.mark.parametrize(
    "spec, proj_str",
    [
        (dict(type="none"), "+proj=longlat +R=6371229"),
        (
            dict(type="eqc", lat_ts=30.0, lat_0=10.0),
            "+proj=eqc +lat_0=10 +lat_ts=30 +to_meter=111198.923448546 +R=6371229",
        ),
        (
            dict(type="aea", lat_0=23, lat_1=29.5, lat_2=45.5, lon_0=-96),
            "+proj=aea +lat_0=23 +lat_1=29.5 +lat_2=45.5 +lon_0=-96 +R=6371229",
        ),
        (dict(type="laea", lat_0=52, lon_0=10, figure="grs80"), "+proj=laea +lat_0=52 +lon_0=10 +ellps=GRS80"),
        (
            dict(type="lcc", lat_1=33, lat_2=45, lon_0=-90, figure="wgs84"),
            "+proj=lcc +lat_0=0 +lat_1=33 +lat_2=45 +lon_0=-90 +ellps=WGS84",
        ),
        (dict(type="lcc", lat_1=40, lon_0=357.5, figure="grib1"), "+proj=lcc +lat_0=0 +lat_1=40 +lon_0=357.5 +R=6367470"),
        (dict(type="mercator", lat_ts=14), "+proj=merc +lat_ts=14 +R=6371229"),
        (
            dict(type="polar_stereographic", lat_0=90, lon_0=-45, figure="wgs84"),
            "+proj=stere +lat_0=90 +lon_0=-45 +ellps=WGS84",
        ),
        (dict(type="ll-to-xyz", figure="wgs84"), "+proj=cart +ellps=WGS84"),
    ],
)
def test_projection_proj_str(spec, proj_str):
    projection = Projection(spec)
    assert projection.proj_str == proj_str

    # the same coordinates as PROJ, from (lon, lat)
    proj = Projection(type="proj", target=proj_str)
    for p in [(0.0, 30.0), (10.0, 45.0), (60.0, 60.0), (20.0, 80.0)]:
        assert proj.fwd(*p) == pytest.approx(projection.fwd(*p), abs=EPS_XY)

    # the same, reversed
    assert Projection({**spec, "reverse": True}).proj_str == proj_str


def test_projection_proj_str_rotation():
    rotation = Projection(type="rotation", south_pole=[10.0, -40.0], rotation_angle=25.0)
    assert rotation.proj_str == "+proj=ob_tran +lon_0=10 +o_lat_p=40 +o_lon_p=25 +o_proj=longlat +R=6371229"


@pytest.mark.parametrize("spec", [dict(type="space_view", h=35785831.0), dict(type="stretch", stretching_factor=2.0)])
def test_projection_proj_str_not_available(spec):
    with pytest.raises(RuntimeError):
        Projection(spec).proj_str


@pytest.mark.parametrize(
    "spec, proj_str, first_last",
    [
        (
            # lambert_lam.grib2
            dict(
                x=[0, 120000, 2500],
                y=[0, 170000, 2500],
                false=[60000.021, 85000.055],
                order="i+j+",
                projection=dict(type="lcc", lat_0=51.967, lat_1=51.967, lon_0=4.9, reverse=True),
            ),
            "+proj=lcc +lat_0=51.967 +lat_1=51.967 +lon_0=4.9 +R=6371229",
            [(51.199433, 4.038959), (52.728068627, 5.790918449)],
        ),
        (
            # PFFPOS000+0000_guyane.grb (points as PROJ)
            dict(
                x=[0, 1440000, 8000],
                y=[0, -1104000, -8000],
                false=[720000.044, -552000.009],
                projection=dict(type="lcc", lat_0=5.12, lat_1=5.12, lon_0=308.65, reverse=True),
            ),
            "+proj=lcc +lat_0=5.12 +lat_1=5.12 +lon_0=308.65 +R=6371229",
            [(10.044953002, 302.098546001), (0.129704986, 315.100527634)],
        ),
        (
            # test_mercator.temp.grib2
            dict(
                x=[0, 2475000, 45000],
                y=[0, 1935000, 45000],
                false=[10569854.248, -1607827.789],
                order="i+j+",
                projection=dict(type="merc", lat_ts=14, reverse=True),
            ),
            "+proj=merc +lat_ts=14 +R=6371229",
            [(14.736453, 262.036499), (31.173065977, 284.975288815)],
        ),
        (
            # CMC_hrdps_east_TMP_TGL_2_ps2.5km_2014081512_P000-00.grib2
            dict(
                x=[0, 1910000, 2500],
                y=[0, 1422500, 2500],
                false=[1122822.896, 5597229.258],
                order="i+j+",
                projection=dict(type="stere", lat_0=90, lat_ts=60, lon_0=280, reverse=True),
            ),
            "+proj=stere +lat_0=90 +lat_ts=60 +lon_0=280 +R=6371229",
            [(38.7017, 268.656832), (50.672962595, 290.67818439)],
        ),
        (
            # opera.grib2
            dict(
                x=[0, 4995000, 5000],
                y=[0, -4745000, -5000],
                false=[1818500.021, -2287499.955],
                projection=dict(type="laea", lat_0=52, lon_0=10, figure="grs80", reverse=True),
            ),
            "+proj=laea +lat_0=52 +lon_0=10 +ellps=GRS80",
            [(66.982142999, -35.034024), (23.942342883, 41.139704951)],
        ),
        (
            # rotated_ll (GRIB2.tmpl, latitudeOfSouthernPole=-40, longitudeOfSouthernPole=10)
            dict(area=[60, 0, 0, 30], grid=[2, 2], projection=dict(south_pole=[10, -40], type="rotation")),
            "+proj=ob_tran +lon_0=10 +o_lat_p=40 +o_proj=longlat +R=6371229",
            [(70.0, 190.0), (41.56076257, 51.93010519)],
        ),
    ],
)
def test_grid_projected(spec, proj_str, first_last):
    grid = Grid(spec)
    assert grid.spec == spec
    assert Grid(grid.spec) == grid

    # PROJ string from the grid, and from its projection spec
    assert grid.projection.proj_str == proj_str
    assert Projection(spec["projection"]).proj_str == proj_str

    # the same points, and as PROJ (on the projection plane)
    grids = [grid]
    if "x" in spec:
        grids.append(Grid({**spec, "projection": dict(type="proj", source=proj_str)}))

    for g in grids:
        lat, lon = g.to_latlons()
        for (lat0, lon0), i in zip(first_last, (0, -1)):
            assert lat[i] == pytest.approx(lat0, abs=1e-6)
            assert (lon[i] - lon0 + 180.0) % 360.0 - 180.0 == pytest.approx(0.0, abs=1e-6)
