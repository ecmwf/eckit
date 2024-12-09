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


#include "eckit/stats/method/MethodT.h"

#include <algorithm>
#include <cmath>

#include "eckit/stats/Field.h"
#include "eckit/stats/detail/AngleT.h"
#include "eckit/stats/detail/CentralMomentsT.h"
#include "eckit/stats/detail/ScalarT.h"


namespace eckit::stats::method {


template <typename T>
inline void eval(T& value, T stat, T missingValue) {
    value = std::isnan(stat) == 0 ? stat : missingValue;
}


template <typename STATS>
void MethodT<STATS>::resize(size_t N) {
    vector_t::resize(N);
}


template <typename STATS>
void MethodT<STATS>::execute(const Field& field) {
    ASSERT(!empty());
    Counter::reset(field);

    ASSERT(field.values().size() == size());

    auto v = field.values().begin();
    for (auto& s : *this) {
        const auto& value = *(v++);
        if (Counter::count(value)) {
            s(value);
        }
    }

    ASSERT(v == field.values().end());
}


template <typename STATS>
void MethodT<STATS>::mean(Field& field) const {
    const auto missingValue = field.missingValue();

    ASSERT(field.values().size() == size());

    Field::values_type statistics(field.values().size());
    auto v = statistics.begin();

    for (auto& s : *this) {
        auto stat = s.mean();
        *(v++)    = std::isnan(stat) == 0 ? stat : missingValue;
    }

    field.update(statistics);
}


template <typename STATS>
void MethodT<STATS>::variance(Field& field) const {
    const auto missingValue = field.missingValue();

    ASSERT(field.values().size() == size());

    Field::values_type statistics(field.values().size());
    auto v = statistics.begin();

    for (auto& s : *this) {
        auto stat = s.variance();
        *(v++)    = std::isnan(stat) == 0 ? stat : missingValue;
    }

    field.update(statistics);
}


template <typename STATS>
void MethodT<STATS>::stddev(Field& field) const {
    const auto missingValue = field.missingValue();

    ASSERT(field.values().size() == size());

    Field::values_type statistics(field.values().size());
    auto v = statistics.begin();

    for (auto& s : *this) {
        auto stat = s.standardDeviation();
        *(v++)    = std::isnan(stat) == 0 ? stat : missingValue;
    }

    field.update(statistics);
}


namespace {
const MethodFactory* METHODS[]{
    new MethodBuilder<MethodT<detail::AngleT<double, detail::AngleScale::DEGREE, detail::AngleSpace::ASYMMETRIC>>>(
        "angle.degree.asymmetric"),
    new MethodBuilder<MethodT<detail::AngleT<double, detail::AngleScale::DEGREE, detail::AngleSpace::SYMMETRIC>>>(
        "angle.degree.symmetric"),
    new MethodBuilder<MethodT<detail::AngleT<double, detail::AngleScale::RADIAN, detail::AngleSpace::ASYMMETRIC>>>(
        "angle.radian.asymmetric"),
    new MethodBuilder<MethodT<detail::AngleT<double, detail::AngleScale::RADIAN, detail::AngleSpace::SYMMETRIC>>>(
        "angle.radian.symmetric"),
    new MethodBuilder<MethodT<detail::CentralMomentsT<double>>>("central-moments"),
    new MethodBuilder<MethodT<detail::ScalarT<double>>>("scalar"),
};
}


}  // namespace eckit::stats::method
