// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/cache/Loader.h"


namespace eckit::cache::loader {


/// No cache file: the user makes the value in private memory, nothing is loaded (mir's "none")
class Memory final : public Loader {
public:

    // -- Constructors

    Memory(const PathName&, const Options&);

    // -- Class methods

    static MemoryUsage estimate(size_t) { return {}; }

    // -- Overridden methods

    const void* address() const override { return nullptr; }
    size_t size() const override { return 0; }
    MemoryUsage footprint() const override { return {}; }

private:

    // -- Overridden methods

    void print(std::ostream&) const override;
};


}  // namespace eckit::cache::loader
