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


#include "mir/stats/statistics/StatisticsT.h"

#include "mir/stats/detail/AngleT.h"
#include "mir/stats/detail/CentralMomentsT.h"
#include "mir/stats/detail/PNormsT.h"
#include "mir/stats/detail/ScalarT.h"


namespace mir {
namespace stats {
namespace statistics {


struct MinMax {};


template <>
void StatisticsT<MinMax>::execute(const data::MIRField& field) {
    Counter::reset(field);

    ASSERT(field.dimensions() == 1);
    for (auto& value : field.values(0)) {
        count(value);
    }
}


template <>
void StatisticsT<MinMax>::print(std::ostream& out) const {
    out << "Statistics[";
    Counter::print(out);
    out << "]";
}


static StatisticsBuilder<
    StatisticsT<detail::AngleT<double, detail::AngleScale::DEGREE, detail::AngleSpace::ASYMMETRIC>>>
    __stats1("angle.degree.asymmetric");
static StatisticsBuilder<StatisticsT<detail::AngleT<double, detail::AngleScale::DEGREE, detail::AngleSpace::SYMMETRIC>>>
    __stats2("angle.degree.symmetric");
static StatisticsBuilder<
    StatisticsT<detail::AngleT<double, detail::AngleScale::RADIAN, detail::AngleSpace::ASYMMETRIC>>>
    __stats3("angle.radian.asymmetric");
static StatisticsBuilder<StatisticsT<detail::AngleT<double, detail::AngleScale::RADIAN, detail::AngleSpace::SYMMETRIC>>>
    __stats4("angle.radian.symmetric");
static StatisticsBuilder<StatisticsT<detail::CentralMomentsT<double>>> __stats5("central-moments");
static StatisticsBuilder<StatisticsT<detail::PNormsT<double>>> __stats6("p-norms");
static StatisticsBuilder<StatisticsT<detail::ScalarT<double>>> __stats7("scalar");
static StatisticsBuilder<StatisticsT<MinMax>> __stats8("min-max");


}  // namespace statistics
}  // namespace stats
}  // namespace mir
