// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File MappedArray.h
// Baudouin Raoult - ECMWF Nov 96

#ifndef eckit_MappedArray_h
#define eckit_MappedArray_h

#include <cstdint>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/os/Semaphore.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// Used to std::map an array to a file

template <class T>
class MappedArray {
public:

    // stl compatibility

    using iterator       = T*;
    using const_iterator = const T*;

    // -- Contructors

    MappedArray(const PathName&, unsigned long);

    MappedArray(const MappedArray&)            = delete;
    MappedArray& operator=(const MappedArray&) = delete;

    // -- Destructor

    ~MappedArray();

    // -- Methods

    void sync();
    void lock() { sem_.lock(); }
    void unlock() { sem_.unlock(); }

    // stl compatibility

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
    unsigned long size_;

    static unsigned long mapped_array_version() { return 1; }

    struct Header {
        uint32_t version_;
        uint32_t headerSize_;
        uint32_t elemSize_;
        Header() : version_(mapped_array_version()), headerSize_(sizeof(Header)), elemSize_(sizeof(T)) {}
        void validate() {
            ASSERT(version_ == mapped_array_version());
            ASSERT(headerSize_ == sizeof(Header));
            ASSERT(elemSize_ == sizeof(T));
        }
    };
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#include "MappedArray.cc"

#endif
