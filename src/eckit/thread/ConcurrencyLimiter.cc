// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#include "eckit/thread/ConcurrencyLimiter.h"

#include <sstream>

#include "eckit/exception/Exceptions.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

ConcurrencyLimiter::ConcurrencyLimiter(const std::string& name, long maxConcurrent) :
    name_(name), maxConcurrent_(maxConcurrent), active_(0) {}

ConcurrencyLimiter::~ConcurrencyLimiter() {}

long ConcurrencyLimiter::active() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return active_;
}

bool ConcurrencyLimiter::tryAcquire(std::string& reason) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (maxConcurrent_ > 0 && active_ >= maxConcurrent_) {
        std::ostringstream oss;
        oss << name_ << ": all " << maxConcurrent_ << " slots in use";
        reason = oss.str();
        return false;
    }

    ++active_;
    return true;
}

void ConcurrencyLimiter::release() {
    std::lock_guard<std::mutex> lock(mutex_);
    ASSERT(active_ > 0);
    --active_;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit
