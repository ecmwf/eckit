# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0

import findlibs

findlibs.load("eckit", "eckitlib")
findlibs.load("eckit_maths", "eckitlib")
findlibs.load("eckit_geo", "eckitlib")

from eckit._utils import configure_ca_bundle_from_certifi
from eckit._utils import configure_projdb
from eckit.geo._eckit_geo import *

configure_projdb()
configure_ca_bundle_from_certifi()

__lib_version__ = version()
__git_sha1__ = git_sha1()
