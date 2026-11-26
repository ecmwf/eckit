// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date May 96

#ifndef eckit_Mutex_h
#define eckit_Mutex_h

#include <pthread.h>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Reentrant Mutex
/// In addition to std::reentrant_mutex this checks valida construction before usage
/// and add an optional tag, both of which can be useful when debugging,
/// especially when dealing with construction before main() execution (eg self-registration classes)
class Mutex {
public:  // methods

    Mutex(char tag = ' ');

    Mutex(const Mutex&)            = delete;
    Mutex& operator=(const Mutex&) = delete;
    Mutex(Mutex&&)                 = delete;
    Mutex& operator=(Mutex&&)      = delete;

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
