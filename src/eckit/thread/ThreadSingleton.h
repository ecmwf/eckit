/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file ThreadSingleton.h
/// @author Baudouin Raoult

#ifndef eckit_ThreadSingleton_h
#define eckit_ThreadSingleton_h

#include <pthread.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/NonCopyable.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
struct NewAlloc0 {
    TYPE* operator()() { return new TYPE(); }
};

template <typename TYPE, typename P1>
struct NewAlloc1 {
    NewAlloc1(const P1& p1) : p1_(p1) {}
    TYPE* operator()() { return new TYPE(p1_); }
    P1 p1_;
};

//----------------------------------------------------------------------------------------------------------------------

template <typename T, typename A = NewAlloc0<T> >
class ThreadSingleton : private NonCopyable {
public:

    ThreadSingleton();
    ThreadSingleton(const A& alloc);

    ~ThreadSingleton();

    T& instance();

private:  // members

    A alloc_;

private:  // class members

    static pthread_once_t once_;
    static pthread_key_t key_;

private:  // class methods

    static void init(void);
    static void cleanUp(void*);
};

//----------------------------------------------------------------------------------------------------------------------

template <typename T, typename A>
pthread_once_t ThreadSingleton<T, A>::once_ = PTHREAD_ONCE_INIT;
template <typename T, typename A>
pthread_key_t ThreadSingleton<T, A>::key_;

template <typename T, typename A>
ThreadSingleton<T, A>::ThreadSingleton() : alloc_(A()) {}

template <typename T, typename A>
ThreadSingleton<T, A>::ThreadSingleton(const A& alloc) : alloc_(alloc) {}

template <typename T, typename A>
ThreadSingleton<T, A>::~ThreadSingleton() {
    ::pthread_once(&once_, init);

    T* value = (T*)::pthread_getspecific(key_);
    if (value) {
        ::pthread_key_delete(key_);
        once_ = PTHREAD_ONCE_INIT;
        delete value;
    }
}

template <typename T, typename A>
T& ThreadSingleton<T, A>::instance() {
    ::pthread_once(&once_, init);

    T* value = (T*)::pthread_getspecific(key_);
    if (!value) {
        value = alloc_();
        THRCALL(::pthread_setspecific(key_, value));
    }

    return *value;
}

template <typename T, typename A>
void ThreadSingleton<T, A>::cleanUp(void* data) {
    delete (T*)data;
    ::pthread_setspecific(key_, 0);
}

template <typename T, typename A>
void ThreadSingleton<T, A>::init() {
    ::pthread_key_create(&key_, cleanUp);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
