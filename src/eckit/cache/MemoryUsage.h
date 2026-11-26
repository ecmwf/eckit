// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>


namespace eckit::cache {


/**
 * Memory used [byte], private and shared between processes (kept apart, as shared memory is used once per host);
 * given as "process[,shared]" (shared defaults to 0)
 */
class MemoryUsage {
public:

    // -- Constructors

    constexpr MemoryUsage() noexcept = default;
    constexpr MemoryUsage(size_t process, size_t shared) noexcept : process_(process), shared_(shared) {}
    explicit MemoryUsage(const std::string&);

    // -- Operators

    constexpr MemoryUsage& operator+=(const MemoryUsage& other) noexcept {
        process_ += other.process_;
        shared_ += other.shared_;
        return *this;
    }

    /// Saturates at zero
    constexpr MemoryUsage& operator-=(const MemoryUsage& other) noexcept {
        process_ = process_ > other.process_ ? process_ - other.process_ : 0;
        shared_  = shared_ > other.shared_ ? shared_ - other.shared_ : 0;
        return *this;
    }

    // -- Methods

    constexpr size_t processMemory() const noexcept { return process_; }
    constexpr size_t sharedMemory() const noexcept { return shared_; }
    constexpr size_t total() const noexcept { return process_ + shared_; }

    /// Either process or shared memory
    constexpr bool exceeds(const MemoryUsage& capacity) const noexcept {
        return process_ > capacity.process_ || shared_ > capacity.shared_;
    }

    // -- Friends

    friend constexpr MemoryUsage operator+(MemoryUsage a, const MemoryUsage& b) noexcept { return a += b; }
    friend constexpr MemoryUsage operator-(MemoryUsage a, const MemoryUsage& b) noexcept { return a -= b; }

    friend constexpr bool operator==(const MemoryUsage& a, const MemoryUsage& b) noexcept {
        return a.process_ == b.process_ && a.shared_ == b.shared_;
    }

    friend constexpr bool operator!=(const MemoryUsage& a, const MemoryUsage& b) noexcept { return !(a == b); }

    friend std::ostream& operator<<(std::ostream&, const MemoryUsage&);

private:

    // -- Members

    size_t process_ = 0;
    size_t shared_  = 0;
};


}  // namespace eckit::cache
