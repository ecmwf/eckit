# (C) Copyright 2025- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

import findlibs

findlibs.load("eckit_geo", "eckitlib")

from eckit._certifi import configure_ca_bundle_from_certifi
from eckit.geo._eckit_geo import *

configure_ca_bundle_from_certifi()

__lib_version__ = version()
__git_sha1__ = git_sha1()
