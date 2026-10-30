// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/cache/Loader.h"


namespace eckit::cache::loader {


/// Content memory-mapped read-only: no copy, pages shared on the host and loaded on demand (default for caches)
class MappedFile final : public Loader {
public:

    // -- Constructors

    MappedFile(const PathName&, const Options&);

    // -- Destructor

    ~MappedFile() override;

    // -- Class methods

    static MemoryUsage estimate(size_t size) { return {0, size}; }

    // -- Overridden methods

    const void* address() const override { return address_; }
    size_t size() const override { return size_; }
    MemoryUsage footprint() const override { return estimate(size_); }

private:

    // -- Members

    void* address_ = nullptr;
    size_t size_   = 0;

    // -- Overridden methods

    void print(std::ostream&) const override;
};


}  // namespace eckit::cache::loader
