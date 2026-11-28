# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

# Searches of many points, results as scipy.sparse.csr_array of distances


import functools
import os
import subprocess
import sys

import pytest

np = pytest.importorskip("numpy")
pytest.importorskip("scipy")

from eckit.geo import Grid  # noqa: E402
from eckit.geo import Search  # noqa: E402


def random_lonlat(n, seed):
    rng = np.random.default_rng(seed)
    return rng.uniform(-180, 180, n), np.degrees(np.arcsin(rng.uniform(-1, 1, n)))


def unit_xyz(lon, lat):
    lon, lat = np.radians(lon), np.radians(lat)
    return np.cos(lat) * np.cos(lon), np.cos(lat) * np.sin(lon), np.sin(lat)


def test_search_indices():
    grid = Grid(grid=[1, 1])
    search = grid.search()

    assert isinstance(search.search_nn((0, 0)), int)
    assert search.search_knn((0, 0), 4).dtype == np.intp

    index, distance = search.search_knn((0.2, 0.1), 4, distance=True)
    assert index[0] == search.search_nn((0.2, 0.1))
    assert np.all(np.diff(distance) >= 0)

    # at the pole, all the points as near
    assert len(search.search_knn_or_same_radius((0, 90), 1)) == 360


def test_csr_search_ll():
    grid = Grid(grid=[1, 1])
    search = grid.search(search="knn", k=4)

    lon, lat = random_lonlat(100, 1)
    a = search.csr_search_ll(lon, lat)

    assert a.shape == (100, len(grid))
    assert np.all(np.diff(a.indptr) == 4)

    # rows as each point's search, by distance
    for i in range(100):
        index, distance = search.search((lon[i], lat[i]), distance=True)
        row = slice(a.indptr[i], a.indptr[i + 1])
        assert np.array_equal(a.indices[row], index)
        assert np.array_equal(a.data[row], distance)

    # k nearest as (n, k) arrays
    assert np.array_equal(a.indices.reshape(-1, 4)[7], search.search((lon[7], lat[7])))

    # in the order given, the same rows
    c = grid.search(search="knn", k=4, query_order="input").csr_search_ll(lon, lat)
    assert np.array_equal(c.indices, a.indices)

    # by index, the same rows sorted
    b = search.csr_search_ll(lon, lat, order="index")
    assert b.has_sorted_indices
    assert np.array_equal(b.indptr, a.indptr)
    for i in range(100):
        row = slice(a.indptr[i], a.indptr[i + 1])
        assert np.array_equal(b.indices[row], np.sort(a.indices[row]))

    with pytest.raises(ValueError):
        search.csr_search_ll(lon, lat, order="other")


def test_csr_search_radius():
    grid = Grid(grid=[1, 1])
    search = grid.search(search="radius", radius=120e3)

    # (0, 0) and its 4 neighbours, and none far from the grid points
    a = search.csr_search_ll([0, 0.5], [0, 0.5])
    assert np.array_equal(np.diff(a.indptr), [5, 4])


def test_csr_search_distance_floor():
    grid = Grid(grid=[1, 1])
    search = grid.search()

    # on grid points, at distance 0
    assert np.all(search.csr_search_ll([0, 10], [0, 20]).data == 1e-6)
    assert np.all(search.csr_search_ll([0, 10], [0, 20], distance_floor=None).data == 0)


def test_from_xyz():
    from scipy.spatial import KDTree

    x, y, z = unit_xyz(*random_lonlat(2000, 2))
    qx, qy, qz = unit_xyz(*random_lonlat(300, 3))

    search = Search.from_xyz(x, y, z, search="knn", k=3)
    assert len(search) == 2000

    # as scipy
    a = search.csr_search_xyz(qx, qy, qz, distance_floor=None)
    distance, index = KDTree(np.column_stack((x, y, z))).query(
        np.column_stack((qx, qy, qz)), k=3
    )
    assert np.array_equal(a.indices.reshape(-1, 3), index)
    assert np.allclose(a.data.reshape(-1, 3), distance)

    # another behaviour, sharing the k-d tree
    nn = search.configured(search="nn")
    assert np.array_equal(nn.csr_search_xyz(qx, qy, qz).indices, index[:, 0])

    with pytest.raises(ValueError):
        search.csr_search_xyz(qx, qy, qz[:-1])

    # (lon, lat, height) queries require points with heights
    with pytest.raises(RuntimeError):
        search.csr_search_llh([0], [0], [0])


def test_heights():
    # points of the same (lon, lat), nearest by height with heights weighing 1000 times
    grid = Grid(longitudes=[7.44, 7.44, 7.45], latitudes=[46.95] * 3, heights=[500, 2500, 520])
    assert grid.type == "unstructured_llh"

    search = grid.search(height_scale_factor=1000)
    assert search.csr_search_llh([7.44], [46.95], [520]).indices[0] == 2
    assert search.search_nn((7.44, 46.95), height=520) == 2
    assert grid.search().search_nn((7.44, 46.95), height=520) == 0

    # (lon, lat) queries would be at height 0
    with pytest.raises(RuntimeError):
        search.search_nn((7.44, 46.95))

    # a height scale factor requires points with heights
    with pytest.raises(RuntimeError):
        Grid(grid=[1, 1]).search(height_scale_factor=1000)


def test_heights_as_scipy():
    # as the k nearest of scipy.spatial.KDTree, of points converted by llh-to-xyz
    from scipy.spatial import KDTree

    from eckit.geo import Projection

    rng = np.random.default_rng(4)
    lon, lat = random_lonlat(2000, 5)
    height = rng.uniform(0, 3000, lon.size)
    qlon, qlat = random_lonlat(300, 6)
    qheight = rng.uniform(0, 3000, qlon.size)

    grid = Grid(longitudes=lon, latitudes=lat, heights=height)
    a = grid.search(search="knn", k=8, height_scale_factor=25).csr_search_llh(
        qlon, qlat, qheight
    )

    llh = Projection(type="llh-to-xyz", height_scale_factor=25)
    tree = KDTree(np.column_stack(llh.fwd(lon, lat, height)))
    _, index = tree.query(np.column_stack(llh.fwd(qlon, qlat, qheight)), k=8)
    assert np.array_equal(a.indices.reshape(-1, 8), index)


def test_estimate():
    usage = Search.estimate(1000)
    assert set(usage) == {"process", "shared"}


# as the eckit-geo-search tests: O288 points searched in O320, 4 nearest (about 10 s in a
# Debug build), single-threaded and on 4 threads if eckit has OpenMP
MANY = """
import hashlib
from eckit.geo import Grid, cache

if cache()["disk"]["search"]["num_threads"] != {num_threads}:
    raise SystemExit(77)

lat, lon = Grid(grid="O288").to_latlons()
a = Grid(grid="O320").search(search="knn", k=4).csr_search_ll(lon, lat)
assert a.shape == (342144, 421120)
assert a.indptr[-1] == 4 * 342144
print(hashlib.md5(a.indptr.tobytes() + a.indices.tobytes()).hexdigest())
"""


@functools.cache
def search_many(num_threads):
    # in a process of its own, as the number of threads is set once; the k-d tree cache
    # file is shared
    env = dict(os.environ, ECKIT_GEO_SEARCH_NUM_THREADS=str(num_threads))
    code = MANY.format(num_threads=num_threads)
    r = subprocess.run(
        [sys.executable, "-c", code], env=env, capture_output=True, text=True
    )
    if r.returncode == 77:
        pytest.skip("eckit without OpenMP")
    assert r.returncode == 0, r.stderr
    return r.stdout.strip()


def test_csr_search_many():
    search_many(1)


def test_csr_search_many_threads():
    assert search_many(4) == search_many(1)
