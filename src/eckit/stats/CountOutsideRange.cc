/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Aug 2016


#include "mir/stats/CountOutsideRange.h"

#include "mir/util/Compare.h"


namespace mir {
namespace stats {


CountOutsideRange::CountOutsideRange(const param::MIRParametrisation& parametrisation) :
    Statistics(parametrisation) {
}


void CountOutsideRange::operator+=(const CountOutsideRange& other) {
    stats_ += other.stats_;
}


Results CountOutsideRange::calculate(const data::MIRField& field) const {
    Results results(field.dimensions());

    util::compare::IsMissingFn isMissing( field.hasMissing()?
                                              field.missingValue() :
                                              std::numeric_limits<double>::quiet_NaN() );

    for (size_t w = 0; w < field.dimensions(); ++w) {

        const std::vector<double>& values = field.values(w);
        size_t missing = 0;


        double lowerLimit = std::numeric_limits<double>::quiet_NaN();
        double upperLimit = std::numeric_limits<double>::quiet_NaN();
        parametrisation_.get("lower-limit", lowerLimit);
        parametrisation_.get("upper-limit", upperLimit);

        stats_.reset(lowerLimit, upperLimit);


        for (size_t i = 0; i < values.size(); ++ i) {

            if (isMissing(values[i])) {
                ++missing;
            } else {
                stats_(values[i]);
            }
        }

        results.counter("count-outside-range", w) = stats_.count();
        results.counter("count-non-missing",   w) = values.size() - missing;
        results.counter("count-missing",       w) = missing;

    }

    return results;
}


namespace {
static StatisticsBuilder<CountOutsideRange> __stats("count-outside-range");
}


}  // namespace stats
}  // namespace mir

