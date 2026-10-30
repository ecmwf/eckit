# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


import os

from eckit.geo import Grid
from eckit.geo import cache
from eckit.geo import cache_purge
from eckit.geo import cache_rmdir


def is_usage(usage) -> bool:
    return set(usage) == {"process", "shared"} and all(isinstance(v, int) and v >= 0 for v in usage.values())


def test_cache_report():
    info = cache()

    # disk control: caching, throwing on a miss, and cache paths (written to the first writable, see conftest)
    control = info["disk"]["control"]
    assert control["caching"] is True
    assert isinstance(control["throw_on_miss"], bool)
    assert os.path.realpath(control["paths"]["writable"][0]) == os.path.realpath(os.environ["ECKIT_GEO_CACHE_PATH"])

    # in-process memory, in the process and shared
    memory = info["memory"]
    assert is_usage(memory["footprint"])
    for kind in ("search", "crop"):
        assert is_usage(memory[kind]["footprint"])
        assert is_usage(memory[kind]["capacity"])
        assert set(memory[kind]["statistics"]) == {"hits", "misses", "insertions", "evictions"}
        assert memory[kind]["policy"] in ("lru", "lfu", "fifo", "largest")

    assert info["disk"]["search"]["loader"] in ("memory", "file", "mapped-file", "mmap", "mapped-anonymous-memory",
                                                "shared-memory", "shmem", "tmp-shared-memory", "tmp-shmem")

    # cache files size (bytes)
    for kind in ("search", "crop", "latlon", "download"):
        assert info["disk"][kind]["size"] >= 0


def test_cache_report_shown():
    info = cache()
    assert isinstance(info, dict)  # values as they are (sizes in bytes)

    # shown as indented text: sizes in binary units, counts as they are
    lines = repr(info).splitlines()
    assert f"    caching: {str(info['disk']['control']['caching']).lower()}" in lines
    assert "  search:" in lines and "    size: " + str(info["memory"]["search"]["size"]) in lines
    assert all(line.split(": ")[-1].split()[-1] in ("B", "KiB", "MiB", "GiB", "TiB")
               for line in lines if line.strip().startswith(("process:", "shared:")) and ": " in line)
    assert str(info) == repr(info)


def test_human_bytes():
    from eckit.geo._eckit_geo import _human_bytes

    assert _human_bytes(0) == "0 B"
    assert _human_bytes(1023) == "1023 B"
    assert _human_bytes(1536) == "1.5 KiB"
    assert _human_bytes(1024**3) == "1.0 GiB"
    assert _human_bytes(5 * 1024**5) == "5120.0 TiB"


def test_cache_report_follows_use():
    grid = Grid(grid=[30, 30])
    before = cache()

    grid.search()

    # a search, in memory and on disk (its cache file)
    after = cache()
    b = before["memory"]["search"]["statistics"]
    a = after["memory"]["search"]["statistics"]
    assert a["hits"] + a["misses"] > b["hits"] + b["misses"]
    assert after["memory"]["search"]["size"] >= 1
    assert after["disk"]["search"]["size"] > 0


def test_cache_purge():
    grid = Grid(grid="O32")
    grid.to_latlons()  # cached (e.g. Gaussian latitudes)
    assert cache()["memory"]["footprint"]["process"] > 0

    cache_purge()
    assert cache()["memory"]["search"]["size"] == 0
    assert cache()["memory"]["crop"]["size"] == 0


def test_cache_rmdir():
    Grid(grid=[30, 30]).search()
    root = cache()["disk"]["control"]["paths"]["writable"][0]
    assert os.path.isdir(root)

    cache_rmdir()
    assert not os.path.exists(root)
    assert cache()["disk"]["search"]["size"] == 0
