# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


import os
import shutil
import tempfile

_cache_path = None


def pytest_configure(config):
    """eckit::geo caches to a temporary directory, not the runtime cache (set before it is resolved, on first use)"""
    global _cache_path
    _cache_path = tempfile.mkdtemp(prefix="eckit_geo_cache.")
    os.environ["ECKIT_GEO_CACHE_PATH"] = _cache_path


def pytest_unconfigure(config):
    if _cache_path is not None:
        shutil.rmtree(_cache_path, ignore_errors=True)
