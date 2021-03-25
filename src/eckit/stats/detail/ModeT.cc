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


#include "mir/stats/detail/ModeT.h"

#include <ostream>

#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace stats {
namespace detail {

namespace {


std::ostream& operator<<(std::ostream& out, const std::vector<double>& v) {
    out << "[";
    auto sep = "";
    for (const auto& e : v) {
        out << sep << e;
        sep = ",";
    }
    out << "]";
    return out;
}


void check_values_and_mins(const std::vector<double>& values, const std::vector<double>& mins) {
    ASSERT(mins.size() + 1 == values.size());

    // Check sorting: {values[0], mins[0], values[1], mins[1], values[2], mins[2], ...}
    for (auto i = values.begin(), j = mins.begin(); j != mins.end(); ++j) {
        ASSERT(*i < *j);
        ASSERT(++i != values.end());
        ASSERT(*j < *i);
    }
}


}  // namespace


ModeReal::ModeReal(const std::vector<double>& values, const std::vector<double>& mins, bool disambiguateMax) :
    Mode(disambiguateMax) {
    check_values_and_mins(values, mins);
    values_ = values;
    mins_   = mins;
}


void ModeReal::setup(const param::MIRParametrisation& param) {
    param.get("mode-disambiguate-max", disambiguateMax_);
    param.get("mode-real-values", values_);
    param.get("mode-real-min", mins_);
    check_values_and_mins(values_, mins_);
}


void ModeReal::print(std::ostream& out) const {
    out << "Mode[mode=" << mode() << ",values=" << values_ << ",mins=" << mins_ << "]";
}


void ModeBoolean::setup(const param::MIRParametrisation& param) {
    param.get("mode-disambiguate-max", disambiguateMax_);
    param.get("mode-boolean-min", min_);
}


void ModeBoolean::print(std::ostream& out) const {
    out << "Mode[mode=" << mode() << ",majority=" << majority_ << "]";
}


void ModeIntegral::setup(const param::MIRParametrisation& param) {
    param.get("mode-disambiguate-max", disambiguateMax_);
}


void ModeIntegral::print(std::ostream& out) const {
    out << "Mode[mode=" << mode() << "]";
}


static ValueStatisticsBuilder<ModeIntegral> __stats_1("mode-integral");
// static ValueStatisticsBuilder<ModeReal> __stats_2("mode-real");  // (no default constructor)
static ValueStatisticsBuilder<ModeBoolean> __stats_3("mode-boolean");


}  // namespace detail
}  // namespace stats
}  // namespace mir
