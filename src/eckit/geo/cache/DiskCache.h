// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/filesystem/PathName.h"


namespace eckit::geo::cache {


class DiskCache {
public:

    // -- Methods

    void rm_cache_root() const { rmdir(root_); }

    const PathName& cache_root() const { return root_; }

protected:

    // -- Constructors

    explicit DiskCache(const PathName& root) : root_{root} {}

    // -- Methods

    void rmdir(const PathName&) const;

private:

    // -- Members

    const PathName root_;
};


}  // namespace eckit::geo::cache
