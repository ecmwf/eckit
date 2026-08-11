# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

import ctypes
import os
import sys
from pathlib import Path

import findlibs


def _configure_eckit_proj() -> None:
    """
    Point eckit's bundled libproj at the proj.db that ships inside the
    eckitlib wheel, using the PROJ per-context API. This scopes the setting
    to eckit's specific libproj instance and does NOT set PROJ_DATA in the
    process env, so it will not interfere with other libraries that also use
    PROJ (pyproj, fiona, GDAL, cartopy, rasterio), which may need their own
    version-matched database.

    The function silently no-ops in any of these cases:
      * The user has set PROJ_DATA / PROJ_LIB explicitly (their choice wins).
      * The eckitlib carrier package is not installed (source / dev build).
      * The wheel does not ship a bundled proj.db (build without PROJ, or
        a partial install).
      * The wheel does not ship a bundled libproj (build without PROJ, or
        eckit is loaded against a system libproj via findlibs' non-PACKAGE
        search sources).
    """
    # (1) User env override wins outright.
    if "PROJ_DATA" in os.environ or "PROJ_LIB" in os.environ:
        return

    # (2) Wheel mode requires the eckitlib carrier package.
    try:
        import eckitlib
    except ImportError:
        return
    eckitlib_file = getattr(eckitlib, "__file__", None)
    if not eckitlib_file:
        return  # namespace package or similar; nothing to locate

    eckitlib_root = Path(eckitlib_file).parent

    # (3) Wheel mode requires a bundled proj.db.
    proj_db = eckitlib_root / "share" / "proj" / "proj.db"
    if not proj_db.is_file():
        return

    # (4) Wheel mode requires the bundled libproj alongside it. We locate it
    #     by full path so the ctypes handle refers to *eckit's* libproj
    #     instance specifically (not pyproj's, if that is also loaded --
    #     auditwheel / delocate mangling makes them distinct libraries).
    if sys.platform == "darwin":
        libdirs = [eckitlib_root / ".dylibs"]
        pattern = "libproj*.dylib"
    elif sys.platform == "win32":
        libdirs = [
            eckitlib_root.parent / "eckitlib.libs",
            eckitlib_root / ".libs",
        ]
        pattern = "libproj*.dll"
    else:  # linux, other posix
        libdirs = [
            eckitlib_root.parent / "eckitlib.libs",
            eckitlib_root / ".libs",
        ]
        pattern = "libproj*.so*"

    libproj_path = next(
        (
            str(match)
            for directory in libdirs
            if directory.is_dir()
            for match in sorted(directory.glob(pattern))
        ),
        None,
    )
    if libproj_path is None:
        return

    # (5) Configure eckit's libproj default context via the API.
    libproj = ctypes.CDLL(libproj_path)

    # int proj_context_set_database_path(PJ_CONTEXT *ctx, const char *dbPath,
    #                                    const char *const *auxDbPaths,
    #                                    const char *const *options);
    # Passing ctx=NULL targets the default context (which is what eckit uses).
    set_db = libproj.proj_context_set_database_path
    set_db.restype = ctypes.c_int
    set_db.argtypes = [
        ctypes.c_void_p,  # PJ_CONTEXT* (NULL = default context)
        ctypes.c_char_p,  # dbPath (file path to proj.db)
        ctypes.c_void_p,  # auxDbPaths (NULL-terminated list; NULL = none)
        ctypes.c_void_p,  # options (NULL = none)
    ]
    set_db(None, str(proj_db).encode(), None, None)

    # void proj_context_set_search_paths(PJ_CONTEXT *ctx, int count,
    #                                    const char *const *paths);
    # Search paths are used to locate proj.ini and grid files (datum shifts,
    # geoid grids, etc.). We point them at the same bundled directory.
    set_paths = libproj.proj_context_set_search_paths
    set_paths.restype = None
    set_paths.argtypes = [
        ctypes.c_void_p,
        ctypes.c_int,
        ctypes.POINTER(ctypes.c_char_p),
    ]
    paths = (ctypes.c_char_p * 1)(str(proj_db.parent).encode())
    set_paths(None, 1, paths)


findlibs.load("eckit", "eckitlib")
findlibs.load("eckit_maths", "eckitlib")
findlibs.load("eckit_geo", "eckitlib")

_configure_eckit_proj()

from eckit._certifi import configure_ca_bundle_from_certifi
from eckit.geo._eckit_geo import *

configure_ca_bundle_from_certifi()

__lib_version__ = version()
__git_sha1__ = git_sha1()
