# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


import os
from pathlib import Path

import pytest
from eckit.geo._eckit_geo import projdb_is_available
from eckit.geo._eckit_geo import projdb_set_search_paths

# Only meaningful when eckit was compiled with PROJ *and* a usable proj.db is
# resolved. projdb_is_available() is otherwise False.
pytestmark = pytest.mark.skipif(
    not projdb_is_available(),
    reason="eckit built without PROJ, or no usable proj.db",
)

BAD_DIR = "/does/not/exist"
BAD_DB = "/does/not/exist/proj.db"


def db_path():
    """Path to the proj.db bundled in the eckitlib wheel, or None."""
    try:
        import eckitlib
    except ImportError:
        return None
    db = Path(eckitlib.__file__).parent / "share" / "proj" / "proj.db"
    return db if db.exists() else None


@pytest.fixture(autouse=True)
def _restore_proj_context():
    """Undo the global PROJ-context / env mutations each test performs."""
    saved = {k: os.environ.get(k) for k in ("PROJ_DATA", "PROJ_LIB")}
    yield
    for k, v in saved.items():
        if v is None:
            os.environ.pop(k, None)
        else:
            os.environ[k] = v

    from eckit.geo import _configure_eckit_proj

    # Recreate the context (drops any test database path, re-resolves the
    # defaults from the now-restored environment), then re-apply the bundled
    # fallback if this build relies on it.
    projdb_set_search_paths("", [])
    _configure_eckit_proj()


def test_projdb_bad_search_paths():
    projdb_set_search_paths(BAD_DB, [BAD_DIR])
    assert not projdb_is_available()


def test_projdb_bad_env():
    os.environ["PROJ_DATA"] = BAD_DIR
    projdb_set_search_paths("", [])
    assert not projdb_is_available()


@pytest.mark.skipif(db_path() is None, reason="no bundled proj.db")
def test_projdb_fallback():
    db = db_path()

    projdb_set_search_paths(BAD_DB, [BAD_DIR])
    assert not projdb_is_available()

    projdb_set_search_paths(str(db), [str(db.parent)])
    assert projdb_is_available()
