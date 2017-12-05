/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author  Baudouin Raoult
/// @author  Tiago Quintino
/// @date    Dec 2017

#ifndef eckit_memory_MMap_h
#define eckit_memory_MMap_h

#include <map>
#include <iosfwd>

#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

class MMap : private eckit::NonCopyable {

public: // methods

    static void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);

    static int munmap(void* addr, size_t length);

    static MMap& instance();

    static void dump(std::ostream& os);

private: // methods

    MMap();

    void increment(size_t len);
    void decrement(size_t len);

private: // members

    long count_;
    long maxCount_;

    size_t length_;
    size_t maxLength_;

    eckit::Mutex mutex_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif
