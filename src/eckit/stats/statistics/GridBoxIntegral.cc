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


#include "eckit/stats/statistics/GridBoxIntegral.h"

#include <limits>

#include "eckit/exception/Exceptions.h"
#include "eckit/repres/Representation.h"
#include "eckit/stats/Field.h"
#include "eckit/util/GridBox.h"


namespace eckit::stats::statistics {


GridBoxIntegral::GridBoxIntegral(const Parametrisation& parametrisation) :
    Statistics(parametrisation), Counter(parametrisation), integral_(std::numeric_limits<double>::quiet_NaN()) {
    reset();
}


void GridBoxIntegral::reset() {
    integral_ = std::numeric_limits<double>::quiet_NaN();
}


void GridBoxIntegral::execute(const Field& field) {

    ASSERT(field.dimensions() == 1);
    const repres::RepresentationHandle rep(field.representation());
    ASSERT(rep);

    const auto boxes   = rep->gridBoxes();
    const auto& values = field.values(0);
    ASSERT(values.size() == rep->numberOfPoints());
    ASSERT(values.size() == boxes.size());

    integral_ = 0.;
    for (size_t i = 0; i < values.size(); ++i) {
        auto value = values[i];
        if (count(value)) {
            integral_ += boxes[i].area() * value;
        }
    }
}


void GridBoxIntegral::print(std::ostream& out) const {
    out << "GridBoxIntegral[" << integral_ << "]";
}


static const StatisticsBuilder<GridBoxIntegral> __stats("grid-box-integral");


}  // namespace eckit::stats::statistics
