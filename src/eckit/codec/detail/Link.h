// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>

namespace eckit {
class PathName;
}

namespace eckit::codec {

struct Link {
    std::string uri;

    const std::string& str() const { return uri; }
    operator bool() const { return !uri.empty(); }
    operator const std::string&() const { return str(); }

    //    bool relative() const;

    //    friend Link operator/( const PathName& dir, const Link& link );
};


}  // namespace eckit::codec
