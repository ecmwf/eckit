// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

// Deprecated, moved to eckit::cache (for existing users, e.g. mir)
#include <string>

#include "eckit/cache/Download.h"


namespace eckit::geo::cache {


class Download {
public:

    [[deprecated("use eckit::cache::Download")]] explicit Download(const PathName& root);

    [[deprecated("use eckit::cache::Download::toCachedPath")]] PathName to_cached_path(const std::string& url) const;

private:

    eckit::cache::Download download_;
};


}  // namespace eckit::geo::cache
