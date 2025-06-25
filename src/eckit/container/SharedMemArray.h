/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   December 2016

#ifndef eckit_SharedMemArray_h
#define eckit_SharedMemArray_h

#include <cstdint>

#include "eckit/memory/NonCopyable.h"
#include "eckit/os/Semaphore.h"

#include "eckit/memory/Padded.h"
#include "eckit/thread/AutoLock.h"

#include "eckit/config/LibEcKit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/memory/MMap.h"
#include "eckit/os/Stat.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Maps an array to shared memory

template <class T>
class SharedMemArray : private NonCopyable {
public:  // types

    typedef T* iterator;
    typedef const T* const_iterator;

public:  // methods

    SharedMemArray(const PathName&, const std::string& shmName, size_t);

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
