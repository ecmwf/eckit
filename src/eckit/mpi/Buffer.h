// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#ifndef eckit_mpi_Buffer_h
#define eckit_mpi_Buffer_h

#include <cstddef>
#include <vector>

namespace eckit::mpi {

//----------------------------------------------------------------------------------------------------------------------

/// Buffer handles colleciton of vector pieces into a larger vector

template <typename DATA_TYPE, typename Allocator = std::allocator<DATA_TYPE>>
struct Buffer {
    using value_type = DATA_TYPE;
    using iterator   = typename std::vector<DATA_TYPE, Allocator>::iterator;

    int cnt;

    std::vector<int> counts;
    std::vector<int> displs;
    std::vector<DATA_TYPE, Allocator> buffer;

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
