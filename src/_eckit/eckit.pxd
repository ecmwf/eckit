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


cdef extern from "eckit/runtime/Main.h" namespace "eckit":
    cdef cppclass Main:
        @staticmethod
        void initialise(int argc, char** argv)


cdef extern from "eckit/filesystem/PathName.h" namespace "eckit":
    cdef cppclass PathName:
        PathName(string)


cdef extern from "eckit/log/JSON.h" namespace "eckit":
    cdef cppclass JSON:
        JSON(std.ostream&)
