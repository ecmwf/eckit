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

#include "eckit/filesystem/PathName.h"


namespace eckit::geo::cache {


class CacheRoot {
public:

    // -- Methods

    void rm_cache_root() const { rmdir(root_); }

    const PathName& cache_root() const { return root_; }

protected:

    // -- Constructors

    explicit CacheRoot(const PathName& root) : root_{root} {}

    // -- Methods

    void rmdir(const PathName&) const;

private:

    // -- Members

    const PathName root_;
};


}  // namespace eckit::geo::cache
