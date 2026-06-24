# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


from libcpp.string cimport string as cppstring

cimport eckit

eckit.eckit_main_initialise()


def version() -> str:
    return eckit.LibEcKit.instance().version()


def git_sha1() -> str:
    return eckit.LibEcKit.instance().gitsha1(40)


def easycurl_setopt(str name, object value) -> None:
    """Set a CURL option default that applies to new handles.

    Args:
        name: option (e.g., "CAINFO", "VERBOSE")
        value: bool, int, or str depending on the option

    Examples:
        easycurl_setopt("VERBOSE", True)
        easycurl_setopt("PORT", 8080)
        easycurl_setopt("CAINFO", "/path/to/ca-bundle.crt")
    """
    cdef cppstring c_name = name.encode("utf-8")
    cdef cppstring c_value

    if isinstance(value, bool):
        eckit.easycurl_setopt_bool(c_name, value)
    elif isinstance(value, int):
        eckit.easycurl_setopt_long(c_name, <long>value)
    elif isinstance(value, str):
        c_value = value.encode("utf-8")
        eckit.easycurl_setopt_string(c_name, c_value)
    else:
        raise TypeError(f"Unsupported type for CURL option value: {type(value)}")


def easycurl_clear_options() -> None:
    """Clear all registered CURL options defaults.

    Resets the CURL options defaults that apply to new handles.
    """
    eckit.easycurl_clear_options()
