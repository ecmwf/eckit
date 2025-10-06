/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_Once_h
#define eckit_Once_h

#include <pthread.h>

#include "eckit/memory/NonCopyable.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <class T>
class Once : private NonCopyable {
public:

    // -- Contructors

    Once();

    // -- Destructor

    ~Once();

    // -- Class methods

    operator T&();

private:

    // -- Members
    T* value_;

    // -- Class members

    static pthread_once_t once_;
    static pthread_mutex_t mutex_;

    // -- Class methods

    static void init(void);
};


//----------------------------------------------------------------------------------------------------------------------

template <class T>
pthread_once_t Once<T>::once_ = PTHREAD_ONCE_INIT;
template <class T>
pthread_mutex_t Once<T>::mutex_;

template <class T>
Once<T>::Once() : value_(0) {}

template <class T>
Once<T>::~Once() {}

template <class T>
Once<T>::operator T&() {
    ::pthread_once(&once_, init);

    ::pthread_mutex_lock(&mutex_);

    if (!value_) {
        value_ = new T();
    }

    ::pthread_mutex_unlock(&mutex_);

    return *value_;
}


template <class T>
void Once<T>::init() {
    pthread_mutexattr_t attr;

    ::pthread_mutexattr_init(&attr);

    //::pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(&mutex_, &attr);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
