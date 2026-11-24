// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/cache/Exceptions.h"


namespace eckit::cache {


CacheError::CacheError(const std::string& what, const CodeLocation& location) :
    CacheError("CacheError", what, location) {}


CacheError::CacheError(const std::string& kind, const std::string& what, const CodeLocation& location) :
    Exception(kind + ": [" + what + "]", location) {}


DownloadError::DownloadError(const std::string& what, const CodeLocation& location) :
    CacheError("DownloadError", what, location) {}


}  // namespace eckit::cache
