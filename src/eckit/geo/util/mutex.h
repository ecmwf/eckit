// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/cache/mutex.h"


namespace eckit::geo::util {


using eckit::cache::util::call_once;
using eckit::cache::util::lock_guard;
using eckit::cache::util::once_flag;
using eckit::cache::util::recursive_mutex;


/**
 * Lock of a mutex by Tag (e.g. a type local to a file), the mutex never destroyed so it can be used from the static
 * initialisation and destruction of other translation units
 */
template <typename Tag>
class lock_type {
public:

    lock_type() : lock_(mutex()) {}

private:

    static recursive_mutex& mutex() {
        static auto* mutex = new recursive_mutex;
        return *mutex;
    }

    lock_guard<recursive_mutex> lock_;
};


}  // namespace eckit::geo::util
