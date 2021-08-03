/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/thread/Mutex.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

Mutex::Mutex(char tag) :
    exists_(false), tag_(tag) {
    pthread_mutexattr_t attr;
    THRCALL(::pthread_mutexattr_init(&attr));
    THRCALL(::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));

    THRCALL(::pthread_mutex_init(&mutex_, &attr));

    exists_ = true;
    THRCALL(::pthread_mutexattr_destroy(&attr));
}

Mutex::~Mutex() {
    THRCALL(::pthread_mutex_destroy(&mutex_));
}

void Mutex::lock(void) {
    if (!exists_) {
        std::cerr << "Mutex used before being constructed" << std::endl;
        ::abort();
    }
    THRCALL(::pthread_mutex_lock(&mutex_));
}

bool Mutex::tryLock(void) {
    if (!exists_) {
        std::cerr << "Mutex used before being constructed" << std::endl;
        ::abort();
    }

    int errcode = ::pthread_mutex_trylock(&mutex_);
    if (errcode == 0)
        return true;

    if (errcode == EBUSY)
        return false;

    ThrCall(errcode, "::pthread_mutex_trylock(&mutex_)", __FILE__, __LINE__, __func__);
    return false;  // never reached
}

void Mutex::unlock(void) {
    if (!exists_) {
        std::cerr << "Mutex used before being constructed" << std::endl;
        ::abort();
    }
    THRCALL(::pthread_mutex_unlock(&mutex_));
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
