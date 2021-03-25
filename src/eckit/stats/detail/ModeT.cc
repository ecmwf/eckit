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


}  // namespace


ModeReal::ModeReal(const std::vector<double>& values, const std::vector<double>& mins, bool disambiguateMax) :
    Mode(disambiguateMax), values_(values), mins_(mins) {
    ASSERT(mins.size() + 1 == values.size());

    // Check sorting: {values[0], mins[0], values[1], mins[1], values[2], mins[2], ...}
    for (auto i = values.begin(), j = mins.begin(); j != mins.end(); ++j) {
        ASSERT(*i < *j);
        ASSERT(++i != values_.end());
        ASSERT(*j < *i);
    }
}


void ModeReal::print(std::ostream& out) const {
    out << "Mode[mode=" << mode() << ",values=" << values_ << ",mins=" << mins_ << "]";
}

void ModeBoolean::print(std::ostream& out) const {
    out << "Mode[mode=" << mode() << ",majority=" << majority_ << "]";
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
