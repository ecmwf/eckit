/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_mpi_Buffer_h
#define eckit_mpi_Buffer_h

#include <cstddef>
#include <vector>

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

/// Buffer handles colleciton of vector pieces into a larger vector

template <typename DATA_TYPE>
struct Buffer {
    typedef DATA_TYPE value_type;
    typedef typename std::vector<DATA_TYPE>::iterator iterator;

    int cnt;

    std::vector<int> counts;
    std::vector<int> displs;
    std::vector<DATA_TYPE> buffer;

    Buffer(size_t size) {
        counts.resize(size);
        displs.resize(size);
    }

    iterator begin() { return buffer.begin(); }
    iterator end() { return buffer.end(); }
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::mpi

#endif
