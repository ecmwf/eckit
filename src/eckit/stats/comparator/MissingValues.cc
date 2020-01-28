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


#include "mir/stats/comparator/MissingValues.h"

#include <memory>
#include <ostream>
#include <sstream>

#include "eckit/exception/Exceptions.h"

#include "mir/data/MIRField.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/Representation.h"


namespace mir {
namespace stats {
namespace comparator {


std::string MissingValues::execute(const data::MIRField& field1, const data::MIRField& field2) {


    // if neither has missing values, don't check for missing
    if (!field1.hasMissing() && !field2.hasMissing()) {
        return "";
    }


    // setup representations (for the iterator)
    repres::RepresentationHandle rep1(field1.representation());
    repres::RepresentationHandle rep2(field2.representation());
    if (!rep1->sameAs(*rep2)) {
        return "\n* different representations, cannot compare";
    }
    if (field1.dimensions() != field2.dimensions()) {
        return "\n* different dimensions, cannot compare";
    }


    // check for when/where missing values are different
    auto missingValue1 = field1.missingValue();
    auto missingValue2 = field2.missingValue();

    std::ostringstream reasons;
    for (size_t d = 0; d < field1.dimensions(); ++d) {
        const auto& values1 = field1.values(d);
        const auto& values2 = field2.values(d);
        ASSERT(values1.size() == values2.size());

        size_t i = 0;
        for (std::unique_ptr<repres::Iterator> it(rep1->iterator()); it->next(); ++i) {
            bool miss1 = values1[i] == missingValue1;
            bool miss2 = values2[i] == missingValue2;
            if (miss1 != miss2) {
                reasons << "\n* " << i << '\t' << it->pointRotated() << '\t'
                        << (miss1 ? "missing" : std::to_string(values1[i])) << '\t'
                        << (miss2 ? "missing" : std::to_string(values2[i]));
            }
        }
    }

    return reasons.str();
}


void MissingValues::print(std::ostream& out) const {
    out << "MissingValues[]";
}


namespace {
static ComparatorBuilder<MissingValues> __stats("missing-values");
}


}  // namespace comparator
}  // namespace stats
}  // namespace mir
