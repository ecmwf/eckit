# (C) Copyright 2025- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


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
