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


#include "mir/stats/Scalar.h"

#include <limits>
#include <sstream>


namespace mir {
namespace stats {


Scalar::Scalar(const param::MIRParametrisation& parametrisation) :
    Statistics(parametrisation) {
}


void Scalar::operator+=(const Scalar& other) {
    stats_ += other.stats_;
}


Results Scalar::calculate(const data::MIRField& field) const {
    Results results(field.dimensions());

    for (size_t w = 0; w < field.dimensions(); ++w) {
        const std::vector<double>& values = field.values(w);

        stats_.reset(field.hasMissing()?
                         field.missingValue() :
                         std::numeric_limits<double>::quiet_NaN());
        for (size_t i = 0; i < values.size(); ++ i) {
            stats_(values[i]);
        }

        results.absoluteQuantity  ("min",      w) = stats_.min();
        results.absoluteQuantity  ("max",      w) = stats_.max();
        results.integerQuantity   ("minIndex", w) = int(stats_.minIndex());
        results.integerQuantity   ("maxIndex", w) = int(stats_.maxIndex());

        results.absoluteQuantity     ("mean",     w) = stats_.mean();
        results.absoluteQuantity2    ("variance", w) = stats_.variance();
        results.absoluteQuantity     ("stddev",   w) = stats_.standardDeviation();
        results.uncomparableQuantity ("skewness", w) = stats_.skewness();
        results.uncomparableQuantity ("kurtosis", w) = stats_.kurtosis();

        results.absoluteQuantity  ("normL1", w) = stats_.normL1();
        results.absoluteQuantity2 ("normL2", w) = stats_.normL2();
        results.absoluteQuantity  ("normLi", w) = stats_.normLinfinity();

        results.counter("count-non-missing", w) = stats_.countNonMissing();
        results.counter("count-missing",     w) = stats_.countMissing();

    }

    return results;
}


namespace {
static StatisticsBuilder<Scalar> __stats("scalar");
}


}  // namespace stats
}  // namespace mir

