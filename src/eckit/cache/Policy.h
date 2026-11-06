// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "eckit/cache/MemoryUsage.h"


namespace eckit::cache {


/**
 * Which cached values a MemoryCache evicts first, of those not in use, by name: lru (default), lfu, fifo, largest
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

    // -- Constructors

    Policy() = default;

    Policy(const Policy&) = delete;
    Policy(Policy&&)      = delete;

    // -- Destructor

    virtual ~Policy() = default;

    // -- Operators

    Policy& operator=(const Policy&) = delete;
    Policy& operator=(Policy&&)      = delete;

    // -- Methods

    /// If a is evicted before b
    virtual bool before(const Entry& a, const Entry& b) const = 0;

    const std::string& name() const { return name_; }

    // -- Class methods

    static std::unique_ptr<Policy> build(const std::string& name);

    static std::vector<std::string> names();

private:

    // -- Members

    std::string name_;
};


}  // namespace eckit::cache
