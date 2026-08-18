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
    Point eckit's PROJ to the proj.db in the eckitlib wheel, so projections work out of the box.

    All the policy is in LibEcKitGeo::ensureProjDatabase. Here we only discover the candidate
    bundled location (it knows where pip installed the eckitlib package).
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

    if not ensure_proj_database(str(proj_dir / "proj.db"), [str(proj_dir)]):
        import warnings

        warnings.warn(
            f"Could not find bundled proj.db at {proj_dir / 'proj.db'}, projection support is limited. Please install PROJ and/or set the PROJ_DATA environment variable."
        )


_configure_eckit_proj()

from eckit._certifi import configure_ca_bundle_from_certifi
from eckit.geo._eckit_geo import *

configure_ca_bundle_from_certifi()

__lib_version__ = version()
__git_sha1__ = git_sha1()
