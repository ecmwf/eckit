# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

from pathlib import Path

import findlibs

findlibs.load("eckit", "eckitlib")
findlibs.load("eckit_maths", "eckitlib")
findlibs.load("eckit_geo", "eckitlib")


def _configure_eckit_proj() -> None:
    """
    Point eckit's PROJ subsystem at the proj.db bundled inside the eckitlib
    wheel, so PROJ-backed projections work out of the box.

    All the policy -- respecting PROJ_DATA/PROJ_LIB, deferring to a database
    PROJ can already find (e.g. a system install), and applying the fallback
    only to eckit's own libproj instance -- lives in the eckit C++ library
    (LibEcKitGeo::ensureProjDatabase). Here we only discover the candidate
    bundled location, which is something only the Python layer can do (it knows
    where pip installed the eckitlib package). The C++ side is a no-op when the
    bundle, or PROJ support itself, is absent.
    """
    try:
        import eckitlib
    except ImportError:
        return
    eckitlib_file = getattr(eckitlib, "__file__", None)
    if not eckitlib_file:
        return

    proj_dir = Path(eckitlib_file).parent / "share" / "proj"
    from eckit.geo._eckit_geo import ensure_proj_database

    ensure_proj_database(str(proj_dir / "proj.db"), [str(proj_dir)])


_configure_eckit_proj()

from eckit._certifi import configure_ca_bundle_from_certifi
from eckit.geo._eckit_geo import *

configure_ca_bundle_from_certifi()

__lib_version__ = version()
__git_sha1__ = git_sha1()
