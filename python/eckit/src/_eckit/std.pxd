# (C) Copyright 1996- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


from libcpp.string cimport string


cdef extern from "<sstream>" namespace "std" nogil:
    cdef cppclass ostream:
        pass

    cdef cppclass ostringstream(ostream):
        ostringstream()
        string to_string "str" () const
