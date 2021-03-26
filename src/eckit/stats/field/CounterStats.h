/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef mir_method_detail_CounterStats_h
#define mir_method_detail_CounterStats_h

#include "mir/stats/detail/Counter.h"


namespace mir {
namespace method {
namespace detail {


/// Counting statistics on values (min, max, etc.)
struct CounterStats : stats::detail::Counter {
    using Counter::Counter;
    ~CounterStats() override = default;

    virtual double value() const     = 0;
    virtual const char* name() const = 0;
};


struct Maximum final : CounterStats {
    using CounterStats::CounterStats;
    double value() const override { return max(); }
    const char* name() const override { return "maximum"; }
};


struct Minimum final : CounterStats {
    using CounterStats::CounterStats;
    double value() const override { return min(); }
    const char* name() const override { return "minimum"; }
};


struct CountAboveUpperLimit final : CounterStats {
    using CounterStats::CounterStats;
    double value() const override { return double(countAboveUpperLimit()); }
    const char* name() const override { return "count-above-upper-limit"; }
};


struct CountBelowLowerLimit final : CounterStats {
    using CounterStats::CounterStats;
    double value() const override { return double(countBelowLowerLimit()); }
    const char* name() const override { return "count-below-lower-limit"; }
};


struct Count final : CounterStats {
    using CounterStats::CounterStats;
    double value() const override { return double(count() - missing()); }
    const char* name() const override { return "count"; }
};


}  // namespace detail
}  // namespace method
}  // namespace mir


#endif
