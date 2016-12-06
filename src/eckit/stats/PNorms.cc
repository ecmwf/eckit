/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @date Aug 2016


#include "mir/stats/PNorms.h"

#include <limits>
#include <sstream>
#include "mir/util/Compare.h"


namespace mir {
namespace stats {


PNorms::PNorms(const param::MIRParametrisation& parametrisation) :
    Statistics(parametrisation) {
}


void PNorms::operator+=(const PNorms& other) {
    stats_ += other.stats_;
}


void PNorms::calculate(const data::MIRField& field, Results& results) const {
    results.reset();

    util::compare::IsMissingFn isMissing( field.hasMissing()?
                                              field.missingValue() :
                                              std::numeric_limits<double>::quiet_NaN() );

    for (size_t w = 0; w < field.dimensions(); ++w) {

        const std::vector<double>& values = field.values(w);
        size_t missing = 0;

        stats_.reset();
        for (size_t i = 0; i < values.size(); ++ i) {

            if (isMissing(values[i])) {
                ++missing;
            } else {
                stats_(values[i]);
            }
        }

        std::string head;
        if (field.dimensions()>1) {
            std::ostringstream s;
            s << '#' << (w+1) << ' ';
            head = s.str();
        }

        results.set(head + " normL1", stats_.normL1());
        results.set(head + " normL2", stats_.normL2());
        results.set(head + " normLinfinity", stats_.normLinfinity());
        results.set(head + " missing", missing);
    }
}


namespace {
// Name (non-)capitalized according to: https://en.wikipedia.org/wiki/Lp_space
static StatisticsBuilder<PNorms> __stats("scalar-p-norms");
}


}  // namespace stats
}  // namespace mir

