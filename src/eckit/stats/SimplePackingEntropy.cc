/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/stats/SimplePackingEntropy.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/ScopedPtr.h"
#include "eckit/parser/JSON.h"

#include "mir/data/MIRField.h"
#include "mir/stats/StatisticsT.h"
#include "mir/stats/detail/MinMax.h"
#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace stats {


SimplePackingEntropy::SimplePackingEntropy(const param::MIRParametrisation& parametrisation) :
    Statistics(parametrisation),
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


SimplePackingEntropy::~SimplePackingEntropy() = default;


void SimplePackingEntropy::reset() {
    entropy_ = std::numeric_limits<double>::quiet_NaN();
    scale_   = std::numeric_limits<double>::quiet_NaN();
    count_       = 0;
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


void SimplePackingEntropy::execute(const data::MIRField& field) {
    reset();

    ASSERT(field.dimensions() == 1);
    auto& values = field.values(0);

    StatisticsT<detail::MinMax> mm(parametrisation_);
    mm.execute(field);

    ASSERT(count_ > 0);
    ASSERT(count_ != missing_);

    // set/fill buckets and compute entropy
    std::vector<size_t> buckets(bucketCount_);
    scale_ = (bucketCount_ - 1)  / (mm.max() - mm.min());
    buckets.assign(bucketCount_, 0);

    const auto count = double(count_);
    const double one_over_log2 = 1. / M_LN2;

    CounterUnary counter(field);

    for (auto& value : values) {
        if (!counter.missingValue(value)) {
            auto b = size_t((value - mm.min()) * scale_);
            ASSERT(b < bucketCount_);
            buckets[b]++;
        }
    }

    entropy_ = 0.;
    for (auto& bucket : buckets) {
        double p = double(bucket) / count;
        if (p > 0) {
            entropy_ += -p * std::log(p) * one_over_log2;
        }
    }
}


void SimplePackingEntropy::print(std::ostream& out) const {
    out << "SimplePackingEntropy[";
    eckit::JSON j(out);
    j.startObject()
            << "count"   << count_
            << "missing" << missing_
            << "entropy" << entropy()
            << "scale"   << scale()
            << "bucketCount"  << bucketCount();
    j.endObject();
    out << "]";
}


namespace {
static StatisticsBuilder<SimplePackingEntropy> __stats("simple-packing-entropy");
}


}  // namespace stats
}  // namespace mir

