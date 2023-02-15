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

#include "mir/stats/field/CentralMomentStats.h"

#include <ostream>

#include "mir/stats/Field.h"
#include "mir/stats/detail/CentralMomentsT.h"


namespace mir::stats::field {


struct Mean final : CentralMomentStatsT<detail::CentralMomentsT<double>> {
    using CentralMomentStatsT::CentralMomentStatsT;
    double value() const override { return mean(); }
    void print(std::ostream& out) const override { out << "Mean[]"; }
};


struct Variance final : CentralMomentStatsT<detail::CentralMomentsT<double>> {
    using CentralMomentStatsT::CentralMomentStatsT;
    double value() const override { return variance(); }
    void print(std::ostream& out) const override { out << "Variance[]"; }
};


struct Skewness final : CentralMomentStatsT<detail::CentralMomentsT<double>> {
    using CentralMomentStatsT::CentralMomentStatsT;
    double value() const override { return skewness(); }
    void print(std::ostream& out) const override { out << "Skewness[]"; }
};


struct Kurtosis final : CentralMomentStatsT<detail::CentralMomentsT<double>> {
    using CentralMomentStatsT::CentralMomentStatsT;
    double value() const override { return kurtosis(); }
    void print(std::ostream& out) const override { out << "Kurtosis[]"; }
};


struct StandardDeviation final : CentralMomentStatsT<detail::CentralMomentsT<double>> {
    using CentralMomentStatsT::CentralMomentStatsT;
    double value() const override { return standardDeviation(); }
    void print(std::ostream& out) const override { out << "StandardDeviation[]"; }
};


struct Sum final : CentralMomentStatsT<detail::CentralMomentsT<double>> {
    using CentralMomentStatsT::CentralMomentStatsT;
    double value() const override { return mean() * double(Counter::count()); }
    void print(std::ostream& out) const override { out << "Sum[]"; }
};


static const FieldBuilder<Mean> __builder_1("mean");
static const FieldBuilder<Variance> __builder_2("variance");
static const FieldBuilder<Skewness> __builder_3("skewness");
static const FieldBuilder<Kurtosis> __builder_4("kurtosis");
static const FieldBuilder<StandardDeviation> __builder_5("stddev");
static const FieldBuilder<Sum> __builder_6("sum");


}  // namespace mir::stats::field
