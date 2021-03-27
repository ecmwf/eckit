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


void mode_values_and_mins_check(const std::vector<double>& values, const std::vector<double>& mins) {
    ASSERT(mins.size() + 1 == values.size());

    // Check sorting: {values[0], mins[0], values[1], mins[1], values[2], mins[3], values[3], ..., mins[N], values[N+1]}
    for (auto i = values.begin(), j = mins.begin(); j != mins.end(); ++j) {
        ASSERT(*i < *j);
        ASSERT(++i != values.end());
        ASSERT(*j < *i);
    }
}


bool mode_disambiguate_max(const param::MIRParametrisation& param) {
    bool disambiguateMax = true;
    param.get("mode-disambiguate-max", disambiguateMax);
    return disambiguateMax;
}


}  // namespace


ModeIntegral::ModeIntegral(bool disambiguateMax) : Mode(disambiguateMax) {}


ModeIntegral::ModeIntegral(const param::MIRParametrisation& param) : Mode<int>(mode_disambiguate_max(param)) {}


void ModeIntegral::print(std::ostream& out) const {
    out << "Mode[mode=" << mode() << "]";
}


ModeReal::ModeReal(bool disambiguateMax, const std::vector<double>& values, const std::vector<double>& mins) :
    Mode(disambiguateMax) {
    mode_values_and_mins_check(values, mins);
    values_ = values;
    mins_   = mins;
}


ModeReal::ModeReal(const param::MIRParametrisation& param) : Mode<size_t>(mode_disambiguate_max(param)) {
    param.get("mode-real-values", values_);
    param.get("mode-real-min", mins_);
    mode_values_and_mins_check(values_, mins_);
}


void ModeReal::print(std::ostream& out) const {
    out << "Mode[mode=" << mode() << ",values=" << values_ << ",mins=" << mins_ << "]";
}


ModeBoolean::ModeBoolean(bool disambiguateMax, double min) : min_(min), disambiguateMax_(disambiguateMax) {
    reset();
}


ModeBoolean::ModeBoolean(const param::MIRParametrisation& param) {
    param.get("mode-disambiguate-max", disambiguateMax_);
    param.get("mode-boolean-min", min_);
}


void ModeBoolean::print(std::ostream& out) const {
    out << "Mode[mode=" << mode() << ",majority=" << majority_ << "]";
}


}  // namespace detail
}  // namespace stats
}  // namespace mir
