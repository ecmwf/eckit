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

#include "mir/stats/detail/AngleT.h"
#include "mir/stats/detail/CentralMomentsT.h"
#include "mir/stats/detail/PNorms.h"
#include "mir/stats/detail/Scalar.h"


namespace mir {
namespace stats {
namespace comparator {


struct MinMax {
};


template<>
void ComparatorT<MinMax>::execute(const data::MIRField& field1, const data::MIRField& field2) {
    CounterBinary::reset(field1, field2);

    ASSERT(field1.dimensions() == 1);
    ASSERT(field2.dimensions() == 1);

    auto& values1 = field1.values(0);
    auto& values2 = field2.values(0);
    ASSERT(values1.size() == values2.size());

    for (size_t i = 0; i < values1.size(); ++i) {
        count(values1[i], values2[i]);
    }

    if (missingDifferent()) {
        throw eckit::BadValue("Different missing values");
    }
}


template<>
void ComparatorT<MinMax>::print(std::ostream& out) const {
    out << "Comparator[";
    CounterBinary::print(out);
    out << "]";
}


static ComparatorBuilder<ComparatorT<detail::AngleT<detail::AngleScale::DEGREE>>> __stats1("angle-degree");
static ComparatorBuilder<ComparatorT<detail::AngleT<detail::AngleScale::RADIAN>>> __stats2("angle-radian");
static ComparatorBuilder<ComparatorT<detail::CentralMomentsT<double>>> __stats3("central-moments");
static ComparatorBuilder<ComparatorT<detail::PNorms>> __stats4("p-norms");
static ComparatorBuilder<ComparatorT<detail::Scalar>> __stats5("scalar");
static ComparatorBuilder<ComparatorT<MinMax>> __stats6("min-max");


}  // namespace comparator
}  // namespace stats
}  // namespace mir
