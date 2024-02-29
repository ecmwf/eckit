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


/// Mode statistics on values
template <typename STATS>
struct ModeStatsT : detail::Counter, Field, STATS {
    ModeStatsT(const param::MIRParametrisation& param) : Counter(param), Field(param), STATS(param) {}

    double value() const override            = 0;
    void json(eckit::JSON&) const override   = 0;
    void print(std::ostream&) const override = 0;

    void count(const double& value) override {
        if (Counter::count(value)) {
            STATS::operator()(value);
        }
    }

    void reset(double missingValue, bool hasMissing) override {
        Counter::reset(missingValue, hasMissing);
        STATS::reset();
    }
};


}  // namespace mir::stats::field
