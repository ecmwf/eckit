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


namespace mir {
namespace stats {
namespace field {


struct Count final : CounterStats {
    using CounterStats::CounterStats;
    double value() const override { return double(Counter::count() - Counter::missing()); }
    void print(std::ostream& out) const override { out << "Count[]"; }
};


struct CountAboveUpperLimit final : CounterStats {
    using CounterStats::CounterStats;
    double value() const override { return double(countAboveUpperLimit()); }
    void print(std::ostream& out) const override { out << "CountAboveUpperLimit[]"; }
};


struct CountBelowLowerLimit final : CounterStats {
    using CounterStats::CounterStats;
    double value() const override { return double(countBelowLowerLimit()); }
    void print(std::ostream& out) const override { out << "CountBelowLowerLimit[]"; }
};


struct Maximum final : CounterStats {
    using CounterStats::CounterStats;
    double value() const override { return max(); }
    void print(std::ostream& out) const override { out << "Maximum[]"; }
};


struct Minimum final : CounterStats {
    using CounterStats::CounterStats;
    double value() const override { return min(); }
    void print(std::ostream& out) const override { out << "Minimum[]"; }
};


static FieldBuilder<Count> __builder_1("count");
static FieldBuilder<CountAboveUpperLimit> __builder_2("count-above-upper-limit");
static FieldBuilder<CountBelowLowerLimit> __builder_3("count-below-lower-limit");
static FieldBuilder<Maximum> __builder_4("maximum");
static FieldBuilder<Minimum> __builder_5("minimum");


}  // namespace field
}  // namespace stats
}  // namespace mir
