from libcpp.string cimport string


cdef extern from "<sstream>" namespace "std" nogil:
    cdef cppclass ostream:
        pass

    cdef cppclass ostringstream(ostream):
        ostringstream()
        string to_string "str" () const
