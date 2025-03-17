/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/thread/MutexCond.h"
#include "eckit/exception/Exceptions.h"

//----------------------------------------------------------------------------------------------------------------------

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

MutexCond::MutexCond(char tag) : tag_(tag) {
    pthread_mutexattr_t attr;
    pthread_condattr_t cattr;

    THRCALL(::pthread_mutexattr_init(&attr));
    THRCALL(::pthread_condattr_init(&cattr));

    THRCALL(::pthread_mutex_init(&mutex_, &attr));
    THRCALL(::pthread_cond_init(&cond_, &cattr));

    inited_ = true;

    THRCALL(::pthread_mutexattr_destroy(&attr));
    THRCALL(::pthread_condattr_destroy(&cattr));
}

MutexCond::~MutexCond() {
    THRCALL(::pthread_mutex_destroy(&mutex_));

    pthread_cond_destroy(&cond_);  // Don't use THRCALL as some thread may be waiting for that condition
    inited_ = false;
}

void MutexCond::lock() {
    ASSERT(inited_);
    THRCALL(::pthread_mutex_lock(&mutex_));
}

void MutexCond::unlock() {
    ASSERT(inited_);
    THRCALL(::pthread_mutex_unlock(&mutex_));
}

void MutexCond::wait() {
    ASSERT(inited_);
    //	AutoState x('.');
    THRCALL(::pthread_cond_wait(&cond_, &mutex_));
}

bool MutexCond::wait(int sec) {
    ASSERT(inited_);
    //	AutoState x(':');
    timespec timeout = {::time(0) + sec, 0};
    int n            = pthread_cond_timedwait(&cond_, &mutex_, &timeout);
    if (n && n != ETIMEDOUT) {
        THRCALL(n);
    }
    return n == ETIMEDOUT;
}

void MutexCond::signal() {
    ASSERT(inited_);
    pthread_cond_signal(&cond_);
}

void MutexCond::broadcast() {
    ASSERT(inited_);
    pthread_cond_broadcast(&cond_);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
