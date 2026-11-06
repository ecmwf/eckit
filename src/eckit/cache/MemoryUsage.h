// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>


namespace eckit::cache {


/**
 * Memory used (bytes), private and shared between processes (kept apart, as shared memory is used once per host);
 * given as "process[,shared]" (shared defaults to 0)
 */
class MemoryUsage {
public:

    // -- Constructors

    MemoryUsage() = default;
    MemoryUsage(size_t process, size_t shared) : process_(process), shared_(shared) {}
    explicit MemoryUsage(const std::string&);

    // -- Operators

    MemoryUsage& operator+=(const MemoryUsage&);

    /// Saturates at zero
    MemoryUsage& operator-=(const MemoryUsage&);

    explicit operator bool() const { return process_ > 0 || shared_ > 0; }

    // -- Methods

    size_t processMemory() const { return process_; }
    size_t sharedMemory() const { return shared_; }
    size_t total() const { return process_ + shared_; }

    /// Either process or shared memory
    bool exceeds(const MemoryUsage& capacity) const;

    // -- Friends

    friend MemoryUsage operator+(MemoryUsage a, const MemoryUsage& b) { return a += b; }
    friend MemoryUsage operator-(MemoryUsage a, const MemoryUsage& b) { return a -= b; }

    friend bool operator==(const MemoryUsage& a, const MemoryUsage& b) {
        return a.process_ == b.process_ && a.shared_ == b.shared_;
    }

    friend bool operator!=(const MemoryUsage& a, const MemoryUsage& b) { return !(a == b); }

    friend std::ostream& operator<<(std::ostream&, const MemoryUsage&);

private:

    // -- Members

    size_t process_ = 0;
    size_t shared_  = 0;
};


}  // namespace eckit::cache
