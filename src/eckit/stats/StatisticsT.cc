/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/stats/StatisticsT.h"

#include "mir/stats/detail/AngleT.h"
#include "mir/stats/detail/CentralMomentsT.h"
#include "mir/stats/detail/MinMax.h"
#include "mir/stats/detail/PNorms.h"
#include "mir/stats/detail/Scalar.h"


namespace mir {
namespace stats {


template<typename STATS>
StatisticsT<STATS>::StatisticsT(const param::MIRParametrisation& parametrisation) :
    Statistics(parametrisation),
    count_(0),
    missing_(0) {
}


template<typename STATS>
void StatisticsT<STATS>::execute(const data::MIRField& field) {
    CounterUnary counter(field);
    STATS::reset();

    ASSERT(field.dimensions() == 1);
    for (auto& value : field.values(0)) {
        if (!counter.missingValue(value)) {
            STATS::operator()(value);
        }
    }

    count_   = counter.count();
    missing_ = counter.missing();
}


#if 0
template<typename STATS>
void StatisticsT<STATS>::calculate(const data::MIRField& field1, const data::MIRField& field2) {
    CounterBinary counter(field1, field2);
    STATS::reset();

    ASSERT(field1.dimensions() == 1);
    ASSERT(field2.dimensions() == 1);

    auto& values1 = field1.values(0);
    auto& values2 = field2.values(0);
    ASSERT(values1.size() == values2.size());

    for (size_t i = 0; i < values1.size(); ++i) {
        if (!counter.missingValues(values1[i], values2[i])) {
            STATS::operator()(std::abs(values1[i] - values2[i]));
        }
    }

    if (counter.missing1()) {
        throw eckit::BadValue("Different missing values");
    }

    count_   = counter.count();
    missing_ = counter.missing2();
}
#endif


template<typename STATS>
void StatisticsT<STATS>::print(std::ostream& out) const {
    out << "Statistics[count" << count_ << ",missing"  << missing_ << ",";
    STATS::print(out);
    out << "]";
}


static StatisticsBuilder<StatisticsT<detail::AngleT<detail::AngleScale::DEGREE>>> __stats1("angle-degree");
static StatisticsBuilder<StatisticsT<detail::AngleT<detail::AngleScale::RADIAN>>> __stats2("angle-radian");
static StatisticsBuilder<StatisticsT<detail::CentralMomentsT<double>>>            __stats3("central-moments");
static StatisticsBuilder<StatisticsT<detail::MinMax>> __stats4("min-max");
static StatisticsBuilder<StatisticsT<detail::PNorms>> __stats5("p-norms");
static StatisticsBuilder<StatisticsT<detail::Scalar>> __stats6("scalar");



}  // namespace stats
}  // namespace mir

