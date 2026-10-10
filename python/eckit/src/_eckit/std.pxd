# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


from libcpp.string cimport string


cdef extern from "<sstream>" namespace "std" nogil:
    cdef cppclass ostream:
        pass

    cdef cppclass ostringstream(ostream):
        ostringstream()
        string to_string "str" () const
