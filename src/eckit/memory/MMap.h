// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date Dec 2017

#ifndef eckit_memory_MMap_h
#define eckit_memory_MMap_h

#include <sys/types.h>

#include <cstddef>
#include <iosfwd>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class MMap {

public:  // methods

    static void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
    static int munmap(void* addr, size_t length);
    static void info(size_t& count, size_t& size);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
