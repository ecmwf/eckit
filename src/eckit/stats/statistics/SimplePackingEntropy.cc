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


#include "eckit/stats/statistics/SimplePackingEntropy.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include "eckit/config/Parametrisation.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/stats/Field.h"


namespace eckit::stats::statistics {


SimplePackingEntropy::SimplePackingEntropy(const Parametrisation& parametrisation) :
    Statistics(parametrisation),
    Counter(parametrisation),
    entropy_(std::numeric_limits<double>::quiet_NaN()),
    scale_(std::numeric_limits<double>::quiet_NaN()),
    bucketCount_(0) {
    reset();

    bucketCount_ = 65536;
    parametrisation.get("entropy-buckets", bucketCount_);

    long bits = 0;
    if (parametrisation.get("entropy-bits", bits)) {
        bucketCount_ = 1L << bits;
    }

    ASSERT(bucketCount_ > 0);
}


void SimplePackingEntropy::reset() {
    entropy_     = std::numeric_limits<double>::quiet_NaN();
    scale_       = std::numeric_limits<double>::quiet_NaN();
    bucketCount_ = 0;
}


double SimplePackingEntropy::entropy() const {
    return entropy_;
}


double SimplePackingEntropy::scale() const {
    return scale_;
}


size_t SimplePackingEntropy::bucketCount() const {
    return bucketCount_;
}


void SimplePackingEntropy::execute(const Field& field) {
    reset();

    Counter::reset(field);
    for (const auto& value : field.values()) {
        count(value);
    }
    const double _max = max();
    const double _min = min();

    ASSERT(count() > 0);
    ASSERT(count() != missing());

    // set/fill buckets and compute entropy
    scale_ = double(bucketCount_ - 1) / (_max - _min);
    std::vector<size_t> buckets(bucketCount_, 0);

    const auto N               = double(count());
    const double one_over_log2 = 1. / M_LN2;

    Counter::reset(field);
    for (const auto& value : field.values()) {
        if (count(value)) {
            auto b = size_t((value - _min) * scale_);
            ASSERT(b < bucketCount_);
            buckets[b]++;
        }
    }

    entropy_ = 0.;
    for (auto& bucket : buckets) {
        double p = double(bucket) / N;
        if (p > 0) {
            entropy_ += -p * std::log(p) * one_over_log2;
        }
    }
}


void SimplePackingEntropy::print(std::ostream& out) const {
    out << "SimplePackingEntropy[";
    Counter::print(out);
    out << ",entropy=" << entropy() << ",scale=" << scale() << ",bucketCount=" << bucketCount() << "]";
}


static const StatisticsBuilder<SimplePackingEntropy> STATISTICS("simple-packing-entropy");


}  // namespace eckit::stats::statistics
