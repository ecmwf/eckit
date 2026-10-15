# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


cimport eckit_geo
from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector

cimport eckit

eckit.eckit_main_initialise()


cdef extern from *:
    """
    #include "eckit/eckit_config.h"
    #include "eckit/geo/eckit_geo_config.h"

    #if eckit_HAVE_CURL
    inline bool eckit_geo_has_download() { return true; }
    #else
    inline bool eckit_geo_has_download() { return false; }
    #endif

    #if eckit_HAVE_GEO_AREA_SHAPEFILE
    inline bool eckit_geo_has_shapefile() { return true; }
    #else
    inline bool eckit_geo_has_shapefile() { return false; }
    #endif
    """
    bool eckit_geo_has_download()
    bool eckit_geo_has_shapefile()


def features() -> set:
    """Optional features compiled in ("download", "shapefile")"""
    available = {
        "download": eckit_geo_has_download(),
        "shapefile": eckit_geo_has_shapefile(),
    }
    return {name for name, have in available.items() if have}


def version() -> str:
    return eckit_geo.LibEcKitGeo.instance().version()


def git_sha1() -> str:
    return eckit_geo.LibEcKitGeo.instance().gitsha1(40)


def cache_dir() -> str:
    """Cache directory (resolved once, eckit-geo-cache-path;$ECKIT_GEO_CACHE_PATH)"""
    return eckit_geo.LibEcKitGeo.cacheDir()


def cache_dir_purge() -> None:
    eckit_geo.LibEcKitGeo.purgeCacheDir()


def projdb_is_available() -> bool:
    """
    If PROJ has a usable database.
    """
    return eckit_geo.LibEcKitGeo.projdb_is_available()


def projdb_set_search_paths(db_path, search_paths=()) -> None:
    """
    Point eckit's PROJ context at a proj.db and its auxiliary search paths.

    Affects only eckit's libproj instance (per-context PROJ API); never leaks
    into other PROJ users in the process (pyproj, GDAL, ...). Call before any
    PROJ-backed projection is created, then re-check projdb_is_available().
    """
    cdef vector[string] _search_paths
    cdef string _db_path
    for p in search_paths:
        _search_paths.push_back(p.encode("utf-8") if isinstance(p, str) else p)
    _db_path = db_path.encode("utf-8") if isinstance(db_path, str) else db_path
    eckit_geo.LibEcKitGeo.projdb_set_search_paths(_db_path, _search_paths)


def projdb_reset() -> None:
    """
    Recreate eckit's PROJ context, (re-)resolves proj.db from env PROJ_DATA/PROJ_LIB
    and the compiled-in defaults, dropping any search paths set).
    """
    eckit_geo.LibEcKitGeo.projdb_reset()


def _spec_str(spec) -> str:
    from yaml import dump

    def _safe_str(value):
        if isinstance(value, str):
            return value
        if isinstance(value, (bytes, bytearray)):
            return value.decode("utf-8")
        if isinstance(value, dict):
            return {_safe_str(k): _safe_str(v) for k, v in value.items()}
        if hasattr(value, "tolist"):
            return _safe_str(value.tolist())
        if isinstance(value, (list, tuple, set, frozenset, range)):
            return [_safe_str(v) for v in value]
        return value

    return dump(_safe_str(spec), default_flow_style=True, width=float("inf")).strip()
