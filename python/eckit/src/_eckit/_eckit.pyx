# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


cimport eckit

eckit.eckit_main_initialise()


def version() -> str:
    return eckit.LibEcKit.instance().version()


def git_sha1() -> str:
    return eckit.LibEcKit.instance().gitsha1(40)
