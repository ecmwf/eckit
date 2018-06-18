/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Oct 2016


#include "mir/stats/SimplePackingEntropy.h"

#include <cmath>
#include <limits>
#include "mir/stats/detail/MinMaxStatistics.h"


namespace mir {
namespace stats {


SimplePackingEntropy::SimplePackingEntropy(const param::MIRParametrisation& parametrisation) :
    Statistics(parametrisation),
    bucketCount_(65536) {

    parametrisation.get("entropy-buckets", bucketCount_);

    long bits = 0;
    if (parametrisation.get("entropy-bits", bits)) {
        bucketCount_ = 1L << bits;
    }

    ASSERT(bucketCount_ > 0);
}


Results SimplePackingEntropy::calculate(const data::MIRField& field) const {
    Results results(field.dimensions());

    // set buckets
    ASSERT(bucketCount_ > 0);
    std::vector<size_t> buckets(bucketCount_);

    // set min/max calculation
    const double missingValue = field.hasMissing()? field.missingValue() : std::numeric_limits<double>::quiet_NaN();
    util::compare::IsMissingFn isMissing(missingValue);
    detail::MinMaxStatistics<double> stats(missingValue);

    for (size_t w = 0; w < field.dimensions(); ++w) {
        ASSERT(field.values(w).size());

        for (auto& value : field.values(w)) {
            stats(value);
        }
        ASSERT(stats.count() != stats.countMissing());


        // fill buckets and compute entropy
        buckets.assign(bucketCount_, 0);
        double entropy = 0;

        const double minvalue = stats.min();
        const double maxvalue = stats.max();
        const double scale = (bucketCount_ - 1)  / (maxvalue - minvalue);

        const double count = double(stats.count());
        const double one_over_log2 = 1 / M_LN2;

        for (auto& value : field.values(w)) {
            if (!isMissing(value)) {
                size_t b = size_t((value - minvalue) * scale);
                ASSERT(b < bucketCount_);
                buckets[b]++;
            }
        }

        for (auto& bucket : buckets) {
            double p = double(bucket) / count;
            if (p > 0) {
                entropy += -p * std::log(p) * one_over_log2;
            }
        }


        // set statistics results
        results.uncomparableQuantity("entropy", w) = entropy;
        results.uncomparableQuantity("scale",   w) = scale;
        results.absoluteQuantity("min", w) = stats.min();
        results.absoluteQuantity("max", w) = stats.max();
        results.counter("count-non-missing", w) = stats.count();
        results.counter("count-missing",     w) = stats.countMissing();

    }

    return results;
}


void SimplePackingEntropy::print(std::ostream& out) const {
    out << "SimplePackingEntropy[";
    Statistics::print(out);
    out << ",bucketCount=" << bucketCount_
        << "]";
}


namespace {
static StatisticsBuilder<SimplePackingEntropy> __stats("simple-packing-entropy");
}


}  // namespace stats
}  // namespace mir

