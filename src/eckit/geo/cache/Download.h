// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

// Deprecated, moved to eckit::cache (for existing users, e.g. mir)
#include "eckit/cache/Download.h"


namespace eckit::geo::cache {


using Download [[deprecated("use eckit::cache::Download")]] = eckit::cache::Download;


}  // namespace eckit::geo::cache
