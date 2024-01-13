/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#pragma once

#define ECKIT_GEO_ECKIT_THREADS

#if defined(ECKIT_GEO_ECKIT_THREADS)
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#else
#include <mutex>
#endif


namespace eckit::geo::util {


#if defined(ECKIT_GEO_ECKIT_THREADS)


using recursive_mutex = eckit::Mutex;

template <typename T>
using lock_guard = typename eckit::AutoLock<T>;

struct once_flag {
    pthread_once_t once_ = PTHREAD_ONCE_INIT;
};

template <class Callable>
inline void call_once(once_flag& flag, Callable&& fun) {
    pthread_once(&(flag.once_), fun);
}


#else


using std::call_once;
using std::lock_guard;
using std::once_flag;
using std::recursive_mutex;


#endif


}  // namespace eckit::geo::util
