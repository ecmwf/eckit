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
/// @date Dec 2017

#ifndef eckit_memory_MMap_h
#define eckit_memory_MMap_h

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
