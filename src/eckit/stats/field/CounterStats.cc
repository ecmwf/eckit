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


#include "mir/stats/field/CounterStats.h"

#include <ostream>

#include "eckit/log/JSON.h"


namespace mir::stats::field {


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


static const FieldBuilder<Count> __builder_1("count");
static const FieldBuilder<CountAboveUpperLimit> __builder_2("count-above-upper-limit");
static const FieldBuilder<CountBelowLowerLimit> __builder_3("count-below-lower-limit");
static const FieldBuilder<Maximum> __builder_4("maximum");
static const FieldBuilder<Minimum> __builder_5("minimum");


}  // namespace mir::stats::field
