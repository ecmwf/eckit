/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Aug 2016


#include "mir/stats/ScalarCentralMoments.h"

#include <sstream>
#include "mir/data/MIRField.h"
#include "mir/util/Compare.h"


namespace mir {
namespace stats {


ScalarCentralMoments::ScalarCentralMoments(const param::MIRParametrisation& parametrisation) :
    Statistics(parametrisation) {
}

void ScalarCentralMoments::operator+=(const ScalarCentralMoments& other) {
    stats_ += other.stats_;
}


Results ScalarCentralMoments::calculate(const data::MIRField& field) const {
    Results results(field.dimensions());

    util::compare::IsMissingFn isMissing( field.hasMissing()?
                                              field.missingValue() :
                                              std::numeric_limits<double>::quiet_NaN() );

    for (size_t w = 0; w < field.dimensions(); ++w) {

        const std::vector<double>& values = field.values(w);
        size_t missing = 0;

        stats_.reset();
        for (size_t i = 0; i < values.size(); ++ i) {

            if (isMissing(values[i])) {
                ++missing;
            } else {
                stats_(values[i]);
            }
        }

        results.uncomparableQuantity("centralMoment1", w) = stats_.centralMoment1();
        results.uncomparableQuantity("centralMoment2", w) = stats_.centralMoment2();
        results.uncomparableQuantity("centralMoment3", w) = stats_.centralMoment3();
        results.uncomparableQuantity("centralMoment4", w) = stats_.centralMoment4();

        results.absoluteQuantity     ("mean",     w) = stats_.mean();
        results.absoluteQuantity2    ("variance", w) = stats_.variance();
        results.absoluteQuantity     ("stddev",   w) = stats_.standardDeviation();
        results.uncomparableQuantity ("skewness", w) = stats_.skewness();
        results.uncomparableQuantity ("kurtosis", w) = stats_.kurtosis();

        results.counter("count-non-missing", w) = stats_.count();
        results.counter("count-missing",     w) = missing;
    }

    return results;
}


namespace {
static StatisticsBuilder<ScalarCentralMoments> __stats("scalar-central-moments");
}


}  // namespace stats
}  // namespace mir

