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


#include "eckit/stats/field/CounterStats.h"

#include <ostream>

#include "eckit/log/JSON.h"


namespace eckit::stats::field {


struct Count final : CounterStats {
    using CounterStats::CounterStats;
    double value() const override { return double(Counter::count() - Counter::missing()); }
    void json(eckit::JSON& j) const override { json_tv(j, "count", value()); }
    void print(std::ostream& out) const override { out << "Count[" << value() << "]"; }
};


struct CountAboveUpperLimit final : CounterStats {
    using CounterStats::CounterStats;
    double value() const override { return double(countAboveUpperLimit()); }
    void json(eckit::JSON& j) const override { json_tv(j, "count-above-upper-limit", value()); }
    void print(std::ostream& out) const override { out << "CountAboveUpperLimit[" << value() << "]"; }
};


struct CountBelowLowerLimit final : CounterStats {
    using CounterStats::CounterStats;
    double value() const override { return double(countBelowLowerLimit()); }
    void json(eckit::JSON& j) const override { json_tv(j, "count-below-upper-limit", value()); }
    void print(std::ostream& out) const override { out << "CountBelowLowerLimit[" << value() << "]"; }
};


struct Maximum final : CounterStats {
    using CounterStats::CounterStats;
    double value() const override { return max(); }
    void json(eckit::JSON& j) const override { json_tv(j, "maximum", value()); }
    void print(std::ostream& out) const override { out << "Maximum[" << value() << "]"; }
};


struct Minimum final : CounterStats {
    using CounterStats::CounterStats;
    double value() const override { return min(); }
    void json(eckit::JSON& j) const override { json_tv(j, "minimum", value()); }
    void print(std::ostream& out) const override { out << "Minimum[" << value() << "]"; }
};


static const FieldStatisticsFactory* __fieldstatistics[]{
    new FieldStatisticsBuilder<Count>("count"),
    new FieldStatisticsBuilder<CountAboveUpperLimit>("count-above-upper-limit"),
    new FieldStatisticsBuilder<CountBelowLowerLimit>("count-below-lower-limit"),
    new FieldStatisticsBuilder<Maximum>("maximum"),
    new FieldStatisticsBuilder<Minimum>("minimum"),
};


}  // namespace eckit::stats::field
