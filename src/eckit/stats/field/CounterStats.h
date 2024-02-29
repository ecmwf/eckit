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


#pragma once

#include "mir/stats/Field.h"
#include "mir/stats/detail/Counter.h"


namespace mir::stats::field {


/// Counting statistics on values (min, max, etc.)
struct CounterStats : detail::Counter, Field {
    CounterStats(const param::MIRParametrisation& param) : Counter(param), Field(param) {}

    double value() const override            = 0;
    void json(eckit::JSON&) const override   = 0;
    void print(std::ostream&) const override = 0;

    void count(const double& value) override { Counter::count(value); }
    void reset(double missingValue, bool hasMissing) override { Counter::reset(missingValue, hasMissing); }
};


}  // namespace mir::stats::field
