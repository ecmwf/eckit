/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/stats/comparator/ComparatorT.h"

#include <cmath>

#include "mir/stats/detail/AngleT.h"
#include "mir/stats/detail/CentralMomentsT.h"
#include "mir/stats/detail/PNorms.h"
#include "mir/stats/detail/Scalar.h"


namespace mir {
namespace stats {
namespace comparator {


template<typename STATS>
std::string ComparatorT<STATS>::execute(const mir::data::MIRField& field1, const mir::data::MIRField& field2) {
    CounterBinary::reset(field1, field2);
    STATS::reset();

    ASSERT(field1.dimensions() == 1);
    ASSERT(field2.dimensions() == 1);

    auto& values1 = field1.values(0);
    auto& values2 = field2.values(0);
    ASSERT(values1.size() == values2.size());

    for (size_t i = 0; i < values1.size(); ++i) {
        auto diff = STATS::difference(values1[i], values2[i]);
        if (CounterBinary::count(values1[i], values2[i], diff)) {
            STATS::operator()(diff);
        }
    }

    return CounterBinary::check();
}


template<typename STATS>
void ComparatorT<STATS>::print(std::ostream& out) const {
    out << "Comparator[";
    CounterBinary::print(out);
    out << ",";
    STATS::print(out);
    out << "]";
}


struct MinMax {};


template<>
std::string ComparatorT<MinMax>::execute(const data::MIRField& field1, const data::MIRField& field2) {
    CounterBinary::reset(field1, field2);

    ASSERT(field1.dimensions() == 1);
    ASSERT(field2.dimensions() == 1);

    auto& values1 = field1.values(0);
    auto& values2 = field2.values(0);
    ASSERT(values1.size() == values2.size());

    for (size_t i = 0; i < values1.size(); ++i) {
        count(values1[i], values2[i], std::abs(values2[i] - values1[i]));
    }

    return CounterBinary::check();
}


template<>
void ComparatorT<MinMax>::print(std::ostream& out) const {
    out << "Comparator[";
    CounterBinary::print(out);
    out << "]";
}


static ComparatorBuilder<ComparatorT<detail::AngleT<detail::AngleScale::DEGREE, detail::AngleSpace::SYMMETRIC>>> __comp1("angle.degree.symmetric");
static ComparatorBuilder<ComparatorT<detail::AngleT<detail::AngleScale::RADIAN, detail::AngleSpace::SYMMETRIC>>> __comp2("angle.radian.symmetric");
static ComparatorBuilder<ComparatorT<detail::CentralMomentsT<double>>> __comp3("central-moments");
static ComparatorBuilder<ComparatorT<detail::PNorms>> __comp4("p-norms");
static ComparatorBuilder<ComparatorT<detail::Scalar>> __comp5("scalar");
static ComparatorBuilder<ComparatorT<MinMax>> __comp6("min-max");


}  // namespace comparator
}  // namespace stats
}  // namespace mir
