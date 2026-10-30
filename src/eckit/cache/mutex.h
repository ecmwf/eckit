// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <utility>

#include "eckit/eckit_config.h"

#if eckit_THREADS_USE_STL
#include <mutex>
#else
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#endif


/// Threading of eckit::cache, on eckit threads (pthreads) or STL threads (see eckit_THREADS_USE_STL)
namespace eckit::cache::util {


#if eckit_THREADS_USE_STL


using std::call_once;
using std::lock_guard;
using std::once_flag;
using std::recursive_mutex;


#else


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


/// As std::call_once: if fun throws, it is called again on the next call
template <typename Callable>
void call_once(once_flag& flag, Callable&& fun) {
    lock_guard<recursive_mutex> lock(flag.mutex_);

    if (!flag.done_) {
        std::forward<Callable>(fun)();
        flag.done_ = true;
    }
}


#endif


}  // namespace eckit::cache::util
