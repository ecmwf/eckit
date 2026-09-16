// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

// File MutexCond.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef eckit_MutexCond_h
#define eckit_MutexCond_h

#include <pthread.h>

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// A mutex and a condition variable
// for Producer/Consumer architectures

class MutexCond {
public:

    // -- Contructors

    MutexCond(char tag = ' ');

    MutexCond(const MutexCond&)            = delete;
    MutexCond& operator=(const MutexCond&) = delete;
    MutexCond(MutexCond&&)                 = delete;
    MutexCond& operator=(MutexCond&&)      = delete;

    // -- Destructor

    ~MutexCond();

    // -- Methods

    void lock();
    void unlock();
    void wait();
    void signal();
    void broadcast();
    bool wait(int);
    char tag() const { return tag_; }

private:

    // -- Members

    pthread_mutex_t mutex_;
    pthread_cond_t cond_;
    char tag_;
    bool inited_;
};


//-----------------------------------------------------------------------------

}  // namespace eckit

#endif
