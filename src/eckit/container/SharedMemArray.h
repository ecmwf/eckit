// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   December 2016

#ifndef eckit_SharedMemArray_h
#define eckit_SharedMemArray_h

#include <cstdint>

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/memory/MMap.h"
#include "eckit/memory/Padded.h"
#include "eckit/os/Semaphore.h"
#include "eckit/os/Stat.h"
#include "eckit/thread/AutoLock.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Maps an array to shared memory

template <class T>
class SharedMemArray {
public:  // types

    using iterator       = T*;
    using const_iterator = const T*;

public:  // methods

    SharedMemArray(const PathName&, const std::string& shmName, size_t);

    SharedMemArray(const SharedMemArray&)            = delete;
    SharedMemArray& operator=(const SharedMemArray&) = delete;
    SharedMemArray(SharedMemArray&&)                 = delete;
    SharedMemArray& operator=(SharedMemArray&&)      = delete;

    ~SharedMemArray();

    void sync();
    void lock() { sem_.lock(); }
    void unlock() { sem_.unlock(); }

    iterator begin() { return array_; }
    iterator end() { return array_ + size_; }

    const_iterator begin() const { return array_; }
    const_iterator end() const { return array_ + size_; }

    unsigned long size() { return size_; }
    T& operator[](unsigned long n) { return array_[n]; }

private:  // members

    Semaphore sem_;
    void* map_;
    int fd_;

    T* array_;
    size_t size_;

    std::string shmName_;

    static unsigned long shared_mem_array_version() { return 1; }

    struct Header {
        uint32_t version_;
        uint32_t headerSize_;
        uint32_t elemSize_;
        Header() : version_(shared_mem_array_version()), headerSize_(sizeof(Header)), elemSize_(sizeof(T)) {}
        void validate() {
            ASSERT(version_ == shared_mem_array_version());
            ASSERT(headerSize_ == sizeof(Header));
            ASSERT(elemSize_ == sizeof(T));
        }
    };
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#include "SharedMemArray.cc"

#endif
