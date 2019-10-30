/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/stats/method/MethodT.h"

#include "mir/data/MIRField.h"
#include "mir/stats/detail/AngleT.h"
#include "mir/stats/detail/CentralMomentsT.h"
#include "mir/stats/detail/ScalarT.h"


namespace mir {
namespace stats {
namespace method {


template<typename STATS>
void MethodT<STATS>::resize(size_t N) {
    vector_t::resize(N);
}


template<typename STATS>
void MethodT<STATS>::execute(const data::MIRField& field) {
    ASSERT(!empty());
    Counter::reset(field);

    ASSERT(field.dimensions() == 1);
    ASSERT(field.values(0).size() == size());

    auto v = field.values(0).begin();
    for (auto& s : *this) {
        auto& value = *(v++);
        if (Counter::count(value)) {
            s(value);
        }
    }

    ASSERT(v == field.values(0).end());
}


template<typename STATS>
void MethodT<STATS>::mean(data::MIRField& field) const {
    const auto missingValue = field.missingValue();

    ASSERT(field.dimensions() == 1);
    ASSERT(field.values(0).size() == size());

    data::MIRValuesVector statistics(field.values(0).size());
    auto v = statistics.begin();

    for (auto& s : *this) {
        auto& value = *(v++);
        auto stat = s.mean();
        value = stat == stat ? stat : missingValue;
    }

    field.update(statistics, 0, true);
}


template<typename STATS>
void MethodT<STATS>::variance(data::MIRField& field) const {
    const auto missingValue = field.missingValue();

    ASSERT(field.dimensions() == 1);
    ASSERT(field.values(0).size() == size());

    data::MIRValuesVector statistics(field.values(0).size());
    auto v = statistics.begin();

    for (auto& s : *this) {
        auto stat = s.variance();
        *v = stat == stat ? stat : missingValue;
    }

    field.update(statistics, 0, true);
}


template<typename STATS>
void MethodT<STATS>::stddev(data::MIRField& field) const {
    const auto missingValue = field.missingValue();

    ASSERT(field.dimensions() == 1);
    ASSERT(field.values(0).size() == size());

    data::MIRValuesVector statistics(field.values(0).size());
    auto v = statistics.begin();

    for (auto& s : *this) {
        auto stat = s.standardDeviation();
        *v = stat == stat ? stat : missingValue;
    }

    field.update(statistics, 0, true);
}


static MethodBuilder<MethodT<detail::AngleT<double, detail::AngleScale::DEGREE, detail::AngleSpace::ASYMMETRIC>>> __stats1("angle.degree.asymmetric");
static MethodBuilder<MethodT<detail::AngleT<double, detail::AngleScale::DEGREE, detail::AngleSpace::SYMMETRIC>>> __stats2("angle.degree.symmetric");
static MethodBuilder<MethodT<detail::AngleT<double, detail::AngleScale::RADIAN, detail::AngleSpace::ASYMMETRIC>>> __stats3("angle.radian.asymmetric");
static MethodBuilder<MethodT<detail::AngleT<double, detail::AngleScale::RADIAN, detail::AngleSpace::SYMMETRIC>>> __stats4("angle.radian.symmetric");
static MethodBuilder<MethodT<detail::CentralMomentsT<double>>> __stats5("central-moments");
static MethodBuilder<MethodT<detail::ScalarT<double>>> __stats6("scalar");


}  // namespace method
}  // namespace stats
}  // namespace mir

