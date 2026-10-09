// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>


namespace eckit::geo::cache {


/**
 * @brief Memory used (bytes), private to the process or shared between processes (shared memory, page cache)
 * @details Controls cache capacities (as mir's InMemoryCacheUsage), given as "memory[,shared]" (shared defaults to
 * memory).
 */
class MemoryUsage {
public:

    // -- Constructors

    MemoryUsage() = default;
    MemoryUsage(size_t memory, size_t shared) : memory_(memory), shared_(shared) {}
    explicit MemoryUsage(const std::string&);

    // -- Operators

    MemoryUsage& operator+=(const MemoryUsage&);

    /// Saturates at zero
    MemoryUsage& operator-=(const MemoryUsage&);

    explicit operator bool() const { return memory_ > 0 || shared_ > 0; }

    // -- Methods

    size_t memory() const { return memory_; }
    size_t shared() const { return shared_; }
    size_t total() const { return memory_ + shared_; }

    /// Either memory or shared
    bool exceeds(const MemoryUsage& capacity) const;

    std::string str() const;

    // -- Friends

    friend MemoryUsage operator+(MemoryUsage a, const MemoryUsage& b) { return a += b; }
    friend MemoryUsage operator-(MemoryUsage a, const MemoryUsage& b) { return a -= b; }

    friend bool operator==(const MemoryUsage& a, const MemoryUsage& b) {
        return a.memory_ == b.memory_ && a.shared_ == b.shared_;
    }

    friend bool operator!=(const MemoryUsage& a, const MemoryUsage& b) { return !(a == b); }

    friend std::ostream& operator<<(std::ostream&, const MemoryUsage&);

private:

    // -- Members

    size_t memory_ = 0;
    size_t shared_ = 0;
};


}  // namespace eckit::geo::cache
