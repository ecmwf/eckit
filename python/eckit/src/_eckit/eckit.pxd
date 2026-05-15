# (C) Copyright 2025- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


cimport std
from libcpp.string cimport string


cdef extern from "eckit.h":
    void eckit_main_initialise()


cdef extern from "eckit/filesystem/PathName.h" namespace "eckit":
    cdef cppclass PathName:
        PathName(string)


cdef extern from "eckit/log/JSON.h" namespace "eckit":
    cdef cppclass JSON:
        JSON(std.ostream&)


cdef extern from "eckit/io/EasyCURL.h" namespace "eckit":
    void easycurl_setopt_bool(string option_name, bool value)
    void easycurl_setopt_long(string option_name, long value)
    void easycurl_setopt_string(string option_name, string value)
    void easycurl_clear_options()
