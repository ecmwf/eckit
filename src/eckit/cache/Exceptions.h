// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/exception/Exceptions.h"


namespace eckit::cache {


class CacheError : public Exception {
public:

    explicit CacheError(const std::string&, const CodeLocation&);

protected:

    CacheError(const std::string& kind, const std::string&, const CodeLocation&);
};


class DownloadError : public CacheError {
public:

    explicit DownloadError(const std::string&, const CodeLocation&);
};


}  // namespace eckit::cache
