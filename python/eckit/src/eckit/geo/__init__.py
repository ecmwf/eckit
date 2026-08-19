# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

import findlibs

findlibs.load("eckit", "eckitlib")
findlibs.load("eckit_maths", "eckitlib")
findlibs.load("eckit_geo", "eckitlib")


def _configure_eckit_proj() -> None:
    """
    Point eckit's PROJ at the proj.db bundled in the eckitlib wheel, so projections work out of the box. Warns if it is not usable.

    Reference:
    https://proj.org/en/stable/resource_files.html

    Resolution order:
      1. If proj.db is immediately resolved (system install, PROJ's compiled-in default search paths, or PROJ_DATA/PROJ_LIB set correctly), do nothing.
      2. If PROJ_DATA or PROJ_LIB aren't set in the environment, set fallback to the proj.db + search paths bundled in the eckitlib wheel, then re-check.

    projdb_is_available() validates proj.db, which catches a missing/unreadable/incompatible database. No-op when eckit was built without PROJ.
    """
    import warnings
    from os import environ
    from pathlib import Path

    from eckit.geo._eckit_geo import projdb_is_available
    from eckit.geo._eckit_geo import projdb_set_search_paths

    msg_footer = "Projection support is limited. Please install PROJ and/or set the PROJ_DATA environment variable."

    if projdb_is_available():
        return

    if "PROJ_DATA" in environ or "PROJ_LIB" in environ:
        warnings.warn(
            "PROJ_DATA/PROJ_LIB is set but does not point at a usable proj.db. Projection "
            "support is limited. Please point it at a proj.db compatible with the linked PROJ, "
            "or unset it to use the bundled database."
        )
        return

    try:
        import eckitlib

        proj_dir = Path(eckitlib.__file__).parent / "share" / "proj"
    except (ImportError, AttributeError, TypeError):
        warnings.warn(
            f"Could not locate the eckitlib module for the bundled proj.db. {msg_footer}"
        )
        return

    projdb_set_search_paths(str(proj_dir / "proj.db"), [str(proj_dir)])

    if not projdb_is_available():
        warnings.warn(
            f"Could not find a usable bundled proj.db at {proj_dir / 'proj.db'}. {msg_footer}"
        )


_configure_eckit_proj()

from eckit._certifi import configure_ca_bundle_from_certifi
from eckit.geo._eckit_geo import *

configure_ca_bundle_from_certifi()

__lib_version__ = version()
__git_sha1__ = git_sha1()
