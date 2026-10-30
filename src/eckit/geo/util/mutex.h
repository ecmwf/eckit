// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/cache/mutex.h"


namespace eckit::geo::util {


using eckit::cache::util::call_once;
using eckit::cache::util::lock_guard;
using eckit::cache::util::once_flag;
using eckit::cache::util::recursive_mutex;


}  // namespace eckit::geo::util
