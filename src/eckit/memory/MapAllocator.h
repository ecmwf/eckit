/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File MapAllocator.h
// MARS (Baudouin Raoult) - ECMWF Nov 01

#ifndef eckit_MapAllocator_h
#define eckit_MapAllocator_h

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/NonCopyable.h"
#include "eckit/types/Types.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class MapAllocatorTooSmall : public Exception {
public:

    MapAllocatorTooSmall(size_t, size_t);
};

//----------------------------------------------------------------------------------------------------------------------

class MapAllocator : private eckit::NonCopyable {
public:

    MapAllocator(size_t);

    ~MapAllocator();

    void* allocate(size_t);
    void deallocate(void*);

private:  // members

    int fd_;
    void* addr_;
    char* next_;
    size_t length_;
    size_t left_;
    Ordinal count_;

    MapAllocator* more_;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
