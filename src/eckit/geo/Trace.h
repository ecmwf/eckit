/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


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
