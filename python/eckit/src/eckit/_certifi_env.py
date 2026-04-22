# (C) Copyright 2025- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

from os import environ


def configure_ca_bundle_from_certifi() -> None:
    """Set libcurl-compatible CA bundle env vars from certifi.

    This must run before loading shared libraries that rely on libcurl.
    """

    try:
        import certifi
    except ImportError:
        return

    ca_bundle = certifi.where()
    if ca_bundle:
        environ.setdefault("CURL_CA_BUNDLE", ca_bundle)
