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


void Scalar::calculate(const data::MIRField& field, Results& results) const {
    results.reset();

    for (size_t w = 0; w < field.dimensions(); ++w) {
        const std::vector<double>& values = field.values(w);

        stats_.reset(field.hasMissing()?
                         field.missingValue() :
                         std::numeric_limits<double>::quiet_NaN());
        for (size_t i = 0; i < values.size(); ++ i) {
            stats_(values[i]);
        }

        std::string head;
        if (field.dimensions()>1) {
            std::ostringstream s;
            s << '#' << (w+1) << ' ';
            head = s.str();
        }

        results.set(head + "min",               stats_.min());
        results.set(head + "max",               stats_.max());
        results.set(head + "minIndex",          stats_.minIndex());
        results.set(head + "maxIndex",          stats_.maxIndex());

        results.set(head + "mean",              stats_.mean());
        results.set(head + "variance",          stats_.variance());
        results.set(head + "skewness",          stats_.skewness());
        results.set(head + "kurtosis",          stats_.kurtosis());
        results.set(head + "standardDeviation", stats_.standardDeviation());

        results.set(head + "normL1",            stats_.normL1());
        results.set(head + "normL2",            stats_.normL2());
        results.set(head + "normLinfinity",     stats_.normLinfinity());

        results.set(head + "count",   stats_.countNonMissing());
        results.set(head + "missing", stats_.countMissing());

    }
}


namespace {
static StatisticsBuilder<Scalar> __stats("scalar");
}


}  // namespace stats
}  // namespace mir

