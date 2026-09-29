// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File MapAllocator.h
// MARS (Baudouin Raoult) - ECMWF Nov 01

#ifndef eckit_MapAllocator_h
#define eckit_MapAllocator_h

#include "eckit/exception/Exceptions.h"
#include "eckit/types/Types.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class MapAllocatorTooSmall : public Exception {
public:

    MapAllocatorTooSmall(size_t, size_t);
};

//----------------------------------------------------------------------------------------------------------------------

class MapAllocator {
public:

    MapAllocator(size_t);

    MapAllocator(const MapAllocator&)            = delete;
    MapAllocator& operator=(const MapAllocator&) = delete;
    MapAllocator(MapAllocator&&)                 = delete;
    MapAllocator& operator=(MapAllocator&&)      = delete;

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
