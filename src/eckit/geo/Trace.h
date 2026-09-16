// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <string>

#include "eckit/log/Seconds.h"
#include "eckit/log/Timer.h"


namespace eckit {
class ResourceUsage;
}


namespace eckit::geo {


class Trace : public Timer {
public:

    explicit Trace(const std::string&);

    Trace(const Trace&) = delete;
    Trace(Trace&&)      = delete;

    virtual ~Trace() = default;

    void operator=(const Trace&) = delete;
    void operator=(Trace&&)      = delete;

    using Timer::elapsed;
    double elapsed(double t) { return Timer::elapsed() - t; }
    Seconds elapsedSeconds(double t = 0, bool compact = false) { return {elapsed(t), compact}; }
};


struct TraceResourceUsage final : public Trace {
    explicit TraceResourceUsage(const std::string&);

    TraceResourceUsage(const TraceResourceUsage&) = delete;
    TraceResourceUsage(TraceResourceUsage&&)      = delete;

    ~TraceResourceUsage();

    void operator=(const TraceResourceUsage&) = delete;
    void operator=(TraceResourceUsage&&)      = delete;

private:

    ResourceUsage* info_;
};


}  // namespace eckit::geo
