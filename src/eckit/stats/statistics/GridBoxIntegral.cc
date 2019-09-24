/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/stats/statistics/GridBoxIntegral.h"

#include <cmath>
#include <limits>

#include "eckit/exception/Exceptions.h"

#include "mir/api/Atlas.h"
#include "mir/data/MIRField.h"
#include "mir/repres/Representation.h"
#include "mir/util/Angles.h"


namespace mir {
namespace stats {
namespace statistics {


GridBoxIntegral::GridBoxIntegral(const param::MIRParametrisation& parametrisation) :
    Statistics(parametrisation),
    Counter(parametrisation),
    integral_(std::numeric_limits<double>::quiet_NaN()) {
    reset();
}


void GridBoxIntegral::reset() {
    integral_ = std::numeric_limits<double>::quiet_NaN();
}


void GridBoxIntegral::execute(const data::MIRField& field) {
    //TODO
    NOTIMP;
}


void GridBoxIntegral::print(std::ostream& out) const {
    out << "GridBoxIntegral[" << integral_ << "]";
}


namespace {
static StatisticsBuilder<GridBoxIntegral> __stats("grid-box-integral");
}


}  // namespace statistics
}  // namespace stats
}  // namespace mir

