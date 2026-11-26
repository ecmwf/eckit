// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include "eckit/cache/MemoryUsage.h"


namespace eckit::cache {


/**
 * Which cached values a MemoryCache evicts first, of those not in use, by name: lru (default), lfu, fifo, largest,
 * largest-per-use, mru, used-once
 */
class Policy {
public:

    // -- Constants

    static constexpr std::string_view DEFAULT = "lru";

    // -- Types

    /// A cached value, as policies see it; times are of the cache clock, which counts uses
    struct Entry {
        MemoryUsage footprint;
        size_t inserted = 0;
        size_t last_use = 0;
        size_t uses     = 0;
    };

    using EvictsBefore = bool (*)(const Entry& a, const Entry& b) noexcept;

    // -- Methods

    constexpr bool evicts_before(const Entry& a, const Entry& b) const noexcept { return evicts_before_(a, b); }

    constexpr std::string_view name() const noexcept { return name_; }

    // -- Class methods

    static Policy build(std::string_view name);

    static std::vector<std::string> names();

private:

    // -- Constructors

    constexpr Policy(std::string_view name, EvictsBefore evicts_before) : name_(name), evicts_before_(evicts_before) {}

    // -- Members

    std::string_view name_;
    EvictsBefore evicts_before_;
};


}  // namespace eckit::cache
