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


#include "eckit/stats/comparator/ComparatorT.h"

#include <cmath>
#include <ostream>

#include "eckit/exception/Exceptions.h"
#include "eckit/stats/Field.h"
#include "eckit/stats/detail/AngleT.h"
#include "eckit/stats/detail/CentralMomentsT.h"
#include "eckit/stats/detail/PNormsT.h"
#include "eckit/stats/detail/ScalarT.h"


namespace eckit::stats::comparator {


template <typename STATS>
std::string ComparatorT<STATS>::execute(const Field& field1, const Field& field2) {
    CounterBinary::reset(field1, field2);
    STATS::reset();

    ASSERT(field1.dimensions() == 1);
    ASSERT(field2.dimensions() == 1);

    const auto& values1 = field1.values(0);
    const auto& values2 = field2.values(0);
    ASSERT(values1.size() == values2.size());

    for (size_t i = 0; i < values1.size(); ++i) {
        auto diff = STATS::difference(values1[i], values2[i]);
        if (CounterBinary::count(values1[i], values2[i], diff)) {
            STATS::operator()(diff);
        }
    }

    return CounterBinary::check();
}


template <typename STATS>
void ComparatorT<STATS>::print(std::ostream& out) const {
    out << "Comparator[";
    CounterBinary::print(out);
    out << ",";
    STATS::print(out);
    out << "]";
}


struct MinMax {};


template <>
std::string ComparatorT<MinMax>::execute(const Field& field1, const Field& field2) {
    CounterBinary::reset(field1, field2);

    ASSERT(field1.dimensions() == 1);
    ASSERT(field2.dimensions() == 1);

    const auto& values1 = field1.values(0);
    const auto& values2 = field2.values(0);
    ASSERT(values1.size() == values2.size());

    for (size_t i = 0; i < values1.size(); ++i) {
        count(values1[i], values2[i], std::abs(values2[i] - values1[i]));
    }

    return CounterBinary::check();
}


template <>
void ComparatorT<MinMax>::print(std::ostream& out) const {
    out << "Comparator[";
    CounterBinary::print(out);
    out << "]";
}


static const ComparatorBuilder<
    ComparatorT<detail::AngleT<double, detail::AngleScale::DEGREE, detail::AngleSpace::SYMMETRIC>>>
    __comp1("angle-degree");
static const ComparatorBuilder<
    ComparatorT<detail::AngleT<double, detail::AngleScale::RADIAN, detail::AngleSpace::SYMMETRIC>>>
    __comp2("angle-radian");
static const ComparatorBuilder<ComparatorT<detail::CentralMomentsT<double>>> __comp3("central-moments");
static const ComparatorBuilder<ComparatorT<detail::PNormsT<double>>> __comp4("p-norms");
static const ComparatorBuilder<ComparatorT<detail::ScalarT<double>>> __comp5("scalar");
static const ComparatorBuilder<ComparatorT<MinMax>> __comp6("min-max");


}  // namespace eckit::stats::comparator
