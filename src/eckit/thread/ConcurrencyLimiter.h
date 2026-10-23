// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Simon Smart

#pragma once

#include <mutex>
#include <string>

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// Limits the number of activities that run concurrently. An activity takes a slot before it starts, and releases it
/// when it ends. If no slot is free, it is not started.

class ConcurrencyLimiter {
public:

    /// @param[in]  maxConcurrent  Maximum number of slots held at once (<= 0 means unlimited)
    ConcurrencyLimiter(const std::string& name, long maxConcurrent);

    ConcurrencyLimiter(const ConcurrencyLimiter&)            = delete;
    ConcurrencyLimiter& operator=(const ConcurrencyLimiter&) = delete;

    ~ConcurrencyLimiter();

    /// @returns true if a slot was taken (it must later be released), false with a reason if none is free
    bool tryAcquire(std::string& reason);
    void release();

    const std::string& name() const { return name_; }

    long active() const;

    long maxConcurrent() const { return maxConcurrent_; }

private:

    const std::string name_;
    const long maxConcurrent_;

    mutable std::mutex mutex_;
    long active_;
};

//----------------------------------------------------------------------------------------------------------------------

class AutoConcurrencySlot {
public:

    explicit AutoConcurrencySlot(ConcurrencyLimiter& limiter) : limiter_(limiter) {}
    ~AutoConcurrencySlot() { limiter_.release(); }

    AutoConcurrencySlot(const AutoConcurrencySlot&)            = delete;
    AutoConcurrencySlot& operator=(const AutoConcurrencySlot&) = delete;

private:

    ConcurrencyLimiter& limiter_;
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
