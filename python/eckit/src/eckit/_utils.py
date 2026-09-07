# (C) Copyright 2025- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


def configure_projdb() -> None:
    """Point eckit's PROJ at the proj.db bundled in the eckitlib wheel, so projections work out of the box. Warns if it is not usable.

    Reference:
    https://proj.org/en/stable/resource_files.html

    Resolution order:
      1. If proj.db is immediately resolved (system install, compiled-in default search paths, or PROJ_DATA/PROJ_LIB set correctly), do nothing.
      2. If PROJ_DATA or PROJ_LIB aren't set in the environment, set fallback to the proj.db + search paths bundled in the eckitlib wheel, then re-check.

    projdb_is_available() validates proj.db, which catches a missing/unreadable/incompatible database. No-op when eckit was built without PROJ.
    """
    from os import environ
    from pathlib import Path
    from warnings import warn

    from eckit.geo._eckit_geo import projdb_is_available
    from eckit.geo._eckit_geo import projdb_set_search_paths

    msg_footer = "Projection support is limited. Please install PROJ and/or set the PROJ_DATA environment variable."

    if projdb_is_available():
        return

    if "PROJ_DATA" in environ or "PROJ_LIB" in environ:
        warn(
            f"PROJ_DATA/PROJ_LIB does not point at a usable proj.db. {msg_footer} Try unsetting it to use the bundled database."
        )
        return

    try:
        import eckitlib

        proj_dir = Path(eckitlib.__file__).parent / "share" / "proj"
    except (ImportError, AttributeError, TypeError):
        warn(
            f"Could not locate the eckitlib module for the bundled proj.db. {msg_footer}"
        )
        return

    projdb_set_search_paths(str(proj_dir / "proj.db"), [str(proj_dir)])

    if not projdb_is_available():
        warn(
            f"Could not find a usable bundled proj.db at {proj_dir / 'proj.db'}. {msg_footer}"
        )


def configure_ca_bundle_from_certifi() -> None:
    """Set the CA bundle path for curl from certifi.

    This configures the default CA bundle to all new curl handles.
    """

    try:
        import certifi
    except ImportError:
        return

    ca_bundle = certifi.where()
    if ca_bundle:
        from eckit._eckit import easycurl_setopt

        easycurl_setopt("CURLOPT_CAINFO", ca_bundle)
