// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   June 2017

#ifndef eckit_CircularBuffer_h
#define eckit_CircularBuffer_h

#include <limits>

#include "eckit/eckit.h"
#include "eckit/thread/Mutex.h"

namespace eckit {


// A simple class to implement buffers

class CircularBuffer {

public:  // methods

    CircularBuffer(size_t size = 64 * 1024, size_t capactity = std::numeric_limits<size_t>::max());

    CircularBuffer(const CircularBuffer&)            = delete;
    CircularBuffer& operator=(const CircularBuffer&) = delete;
    CircularBuffer(CircularBuffer&&)                 = delete;
    CircularBuffer& operator=(CircularBuffer&&)      = delete;

    ~CircularBuffer();

    size_t write(const void* buffer, size_t length);
    size_t read(void* buffer, size_t length);

    size_t length() const;
    size_t capacity() const;
    size_t size() const;

    void clear();

private:  // members

    mutable Mutex mutex_;

    char* buffer_;

    size_t increment_;
    size_t size_;
    size_t capacity_;

    size_t pos_;
    size_t used_;
};


}  // namespace eckit

#endif
