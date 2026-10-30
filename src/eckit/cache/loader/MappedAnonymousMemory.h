// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/cache/Loader.h"


namespace eckit::cache::loader {


/**
 * Content read into private anonymous mapped pages (as mir's mapped-anonymous-memory): as File, but outside the heap,
 * so large content doesn't fragment it and is returned to the system when released
 */
class MappedAnonymousMemory final : public Loader {
public:

    // -- Constructors

    MappedAnonymousMemory(const PathName&, const Options&);

    // -- Destructor

    ~MappedAnonymousMemory() override;

    // -- Class methods

    static MemoryUsage estimate(size_t size) { return {size, 0}; }

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
