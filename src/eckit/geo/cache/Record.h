/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


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
