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


#include "eckit/stats/comparator/MissingValues.h"

#include <ostream>
#include <sstream>

#include "eckit/exception/Exceptions.h"
#include "eckit/stats/Field.h"


namespace eckit::stats::comparator {


std::string MissingValues::execute(const Field& field1, const Field& field2) {
    std::ostringstream reasons;

    // if neither has missing values, don't check for missing
    if (!field1.hasMissing() && !field2.hasMissing()) {
        return "";
    }

    // check for when/where missing values are different
    auto missingValue1 = field1.missingValue();
    auto missingValue2 = field2.missingValue();

    const auto& values1 = field1.values();
    const auto& values2 = field2.values();
    ASSERT(values1.size() == values2.size());

    for (size_t i = 0; i < values1.size(); ++i) {
        bool miss1 = values1.at(i) == missingValue1;
        bool miss2 = values2.at(i) == missingValue2;

        if (miss1 != miss2) {
            reasons << "\n* " << i << '\t' << (miss1 ? "missing" : std::to_string(values1[i])) << '\t'
                    << (miss2 ? "missing" : std::to_string(values2[i]));
        }
        }

    return reasons.str();
}


void MissingValues::print(std::ostream& out) const {
    out << "MissingValues[]";
}


static const ComparatorBuilder<MissingValues> __stats("missing-values");


}  // namespace eckit::stats::comparator
