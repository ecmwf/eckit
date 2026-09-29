// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>

namespace eckit {
class PathName;
namespace spec {
class Spec;
}
}  // namespace eckit


namespace eckit::geo::cache {


class RecordCache {
public:

    using bytes_t = decltype(sizeof(int));
    using Spec    = spec::Spec;

    virtual ~RecordCache()             = default;
    virtual bytes_t footprint() const  = 0;
    virtual void read(const PathName&) = 0;
};


}  // namespace eckit::geo::cache
