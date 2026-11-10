# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


import os
import shutil
import tempfile

_cache_path = None


def pytest_configure(config):
    """eckit::geo caches to a temporary directory, with pinned settings (set before they are resolved, on first use)"""
    global _cache_path
    _cache_path = tempfile.mkdtemp(prefix="eckit_geo_cache.")
    os.environ.update(
        {
            "ECKIT_GEO_CACHE_PATH": _cache_path,
            "ECKIT_GEO_CACHING": "1",
            "ECKIT_GEO_CACHE_CROP_CAPACITY": "0",
            "ECKIT_GEO_CACHE_CROP_POLICY": "lru",
            "ECKIT_GEO_CACHE_SEARCH_CAPACITY": "1073741824,0",
            "ECKIT_GEO_CACHE_SEARCH_POLICY": "lru",
            "ECKIT_GEO_CACHE_THROW_ON_MISS": "0",
            "ECKIT_GEO_SEARCH_LOADER": "mapped-file",
        }
    )


def pytest_unconfigure(config):
    if _cache_path is not None:
        shutil.rmtree(_cache_path, ignore_errors=True)
