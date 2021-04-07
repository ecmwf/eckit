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


namespace mir {
namespace stats {
namespace field {


/// Central moment statistics on values (mean, stddev, etc.)
template <typename STATS>
struct CentralMomentStatsT : detail::Counter, Field, STATS {
    CentralMomentStatsT(const param::MIRParametrisation& param) : Counter(param), Field(param) {}
    ~CentralMomentStatsT() override = default;

    virtual double value() const override = 0;

    void count(const double& value) {
        if (Counter::count(value)) {
            STATS::operator()(value);
        }
    }

    void reset(double missingValue, bool hasMissing) {
        Counter::reset(missingValue, hasMissing);
        STATS::reset();
    }
};


}  // namespace field
}  // namespace stats
}  // namespace mir
