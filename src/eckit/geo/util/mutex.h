// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#define eckit_GEO_ECKIT_THREADS

#include <map>
#include <memory>
#include <utility>

#if defined(eckit_GEO_ECKIT_THREADS)
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#else
#include <mutex>
#endif


/// Threading of eckit::geo, on eckit threads (pthreads) as production software requires, or on the STL threads
namespace eckit::geo::util {


#if defined(eckit_GEO_ECKIT_THREADS)


using recursive_mutex = eckit::Mutex;


/// As std::lock_guard
template <typename T>
using lock_guard = eckit::AutoLock<T>;


/// As std::once_flag
class once_flag {
public:

    once_flag() = default;

    once_flag(const once_flag&)            = delete;
    once_flag(once_flag&&)                 = delete;
    once_flag& operator=(const once_flag&) = delete;
    once_flag& operator=(once_flag&&)      = delete;

private:

    recursive_mutex mutex_;
    bool done_ = false;

    template <typename Callable>
    friend void call_once(once_flag&, Callable&&);
};


/// As std::call_once: fun is called once, unless it throws (then it is called again on the next call)
template <typename Callable>
void call_once(once_flag& flag, Callable&& fun) {
    lock_guard<recursive_mutex> lock(flag.mutex_);

    if (!flag.done_) {
        std::forward<Callable>(fun)();
        flag.done_ = true;
    }
}


#else


using std::call_once;
using std::lock_guard;
using std::once_flag;
using std::recursive_mutex;


#endif


/// Recursive mutexes by key, created on first use and never released (the number of keys is bounded)
template <typename Key>
class mutex_map {
public:

    recursive_mutex& operator[](const Key& key) {
        lock_guard<recursive_mutex> lock(mutex_);

        auto& m = mutexes_[key];
        if (!m) {
            m = std::make_unique<recursive_mutex>();
        }
        return *m;
    }

private:

    recursive_mutex mutex_;
    std::map<Key, std::unique_ptr<recursive_mutex>> mutexes_;
};


}  // namespace eckit::geo::util
