# SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
# SPDX-License-Identifier: Apache-2.0


from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector


cdef extern from "eckit/geo/LibEcKitGeo.h" namespace "eckit":
    cdef cppclass LibEcKitGeo:
        @staticmethod
        LibEcKitGeo& instance()

        @staticmethod
        void purgeCacheDir() except +

        @staticmethod
        bool projdb_is_available() except +

        @staticmethod
        void projdb_set_search_paths(
            const string& db_path, const vector[string]& search_paths
        ) except +

        string version() except +
        string gitsha1(unsigned int n) except +  # n=40 for full sha1
