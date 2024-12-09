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


#include "eckit/stats/statistics/StatisticsT.h"

#include "eckit/stats/detail/AngleT.h"
#include "eckit/stats/detail/CentralMomentsT.h"
#include "eckit/stats/detail/PNormsT.h"
#include "eckit/stats/detail/ScalarT.h"


namespace eckit::stats::statistics {


struct MinMax {};


template <>
void StatisticsT<MinMax>::execute(const Field& field) {
    Counter::reset(field);

    for (const auto& value : field.values()) {
        count(value);
    }
}


template <>
void StatisticsT<MinMax>::print(std::ostream& out) const {
    out << "Statistics[";
    Counter::print(out);
    out << "]";
}


static const StatisticsFactory* __stats[]{
    new StatisticsBuilder<
        StatisticsT<detail::AngleT<double, detail::AngleScale::DEGREE, detail::AngleSpace::ASYMMETRIC>>>(
        "angle-degree-asymmetric"),
    new StatisticsBuilder<
        StatisticsT<detail::AngleT<double, detail::AngleScale::DEGREE, detail::AngleSpace::SYMMETRIC>>>(
        "angle-degree-symmetric"),
    new StatisticsBuilder<
        StatisticsT<detail::AngleT<double, detail::AngleScale::RADIAN, detail::AngleSpace::ASYMMETRIC>>>(
        "angle-radian-asymmetric"),
    new StatisticsBuilder<
        StatisticsT<detail::AngleT<double, detail::AngleScale::RADIAN, detail::AngleSpace::SYMMETRIC>>>(
        "angle-radian-symmetric"),
    new StatisticsBuilder<StatisticsT<detail::CentralMomentsT<double>>>("central-moments"),
    new StatisticsBuilder<StatisticsT<detail::PNormsT<double>>>("p-norms"),
    new StatisticsBuilder<StatisticsT<detail::ScalarT<double>>>("scalar"),
    new StatisticsBuilder<StatisticsT<MinMax>>("min-max"),
};


}  // namespace eckit::stats::statistics
