/*
 * (C) Copyright 1996-2016 ECMWF.
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
    Results results;

    // set buckets
    ASSERT(bucketCount_ > 0);
    std::vector<size_t> buckets(bucketCount_);

    // set min/max calculation
    const double missingValue = field.hasMissing()? field.missingValue() : std::numeric_limits<double>::quiet_NaN();
    util::compare::IsMissingFn isMissing(missingValue);
    detail::MinMaxStatistics<double> stats(missingValue);

    for (size_t w = 0; w < field.dimensions(); ++w) {
        const std::vector<double>& values = field.values(w);
        ASSERT(values.size());

        std::string head;
        if (field.dimensions()>1) {
            std::ostringstream s;
            s << '#' << (w+1) << ' ';
            head = s.str();
        }


        for (size_t i = 0; i < values.size(); ++i) {
            stats(values[i]);
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

        for (size_t i = 0; i < values.size(); ++i) {
            if (!isMissing(values[i])) {
                size_t b = size_t((values[i] - minvalue) * scale);
                ASSERT(b < bucketCount_);
                buckets[b]++;
            }
        }

        for (size_t i = 0; i < buckets.size(); ++i) {
            double p = double(buckets[i]) / count;
            if (p) {
                entropy += -p * std::log(p) * one_over_log2;
            }
        }


        // set statistics results
        results.set(head + "entropy", entropy);
        results.set(head + "scale",   scale);
        results.set(head + "min",     stats.min());
        results.set(head + "max",     stats.max());
        results.set(head + "count",   stats.count());
        results.set(head + "missing", stats.countMissing());

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

