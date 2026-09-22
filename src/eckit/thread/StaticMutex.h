// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Baudouin Raoult
/// @author Tiago Quintino
/// @date   Nov 2016

#ifndef eckit_StaticMutex_h
#define eckit_StaticMutex_h

#include <pthread.h>


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Class meant to be used only for static mutexes protecting local resources inside a single compilation unit

class StaticMutex {

public:  // methods

    StaticMutex();

    StaticMutex(const StaticMutex&)            = delete;
    StaticMutex& operator=(const StaticMutex&) = delete;
    StaticMutex(StaticMutex&&)                 = delete;
    StaticMutex& operator=(StaticMutex&&)      = delete;

    ~StaticMutex();

    void lock();
    void unlock();

protected:  // members

    void init();

    /// since this will be static memory, it should be initialized to zero by the system

    pthread_mutex_t mutex_;
    bool exists_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
