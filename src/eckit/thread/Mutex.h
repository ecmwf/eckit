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
/// @date May 96

#ifndef eckit_Mutex_h
#define eckit_Mutex_h

#include <pthread.h>

#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Reentrant Mutex
/// In addition to std::reentrant_mutex this checks valida construction before usage
/// and add an optional tag, both of which can be useful when debugging,
/// especially when dealing with construction before main() execution (eg self-registration classes)
class Mutex : private NonCopyable {
public:  // methods

    Mutex(char tag = ' ');

    ~Mutex();

    void lock();
    void unlock();
    bool tryLock();
    char tag() const { return tag_; }

protected:  // members

    pthread_mutex_t mutex_;
    bool exists_;
    char tag_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
