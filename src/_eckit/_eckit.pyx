# (C) Copyright 2025- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.


from cpython.bytes cimport PyBytes_AsString
from libc.stdlib cimport free
from libc.stdlib cimport malloc

cimport eckit


cdef class Args:
    cdef int argc
    cdef char** argv
    cdef list _argv_storage

    def __cinit__(self):
        import sys
        self.argc = len(sys.argv)
        self.argv = <char**> malloc(self.argc * sizeof(char*))
        self._argv_storage = []

        cdef bytes arg_bytes
        for i, arg in enumerate(sys.argv):
            arg_bytes = arg.encode("utf-8")
            self._argv_storage.append(arg_bytes)
            self.argv[i] = PyBytes_AsString(arg_bytes)

    def __dealloc__(self):
        free(self.argv)


cdef public _init():
    args = Args()
    eckit.Main.initialise(args.argc, args.argv)


_init()
