/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @author Pedro Maciel
/// @date Jul 2015


#include "mir/stats/Integral.h"

#include <cmath>
#include <limits>

#include "eckit/exception/Exceptions.h"

#include "mir/api/Atlas.h"
#include "mir/data/MIRField.h"
#include "mir/repres/Representation.h"
#include "mir/util/Angles.h"


namespace mir {
namespace stats {


Integral::Integral(const param::MIRParametrisation& parametrisation) :
    Statistics(parametrisation) {
    reset();
}


void Integral::reset() {
    integral_ = std::numeric_limits<double>::quiet_NaN();
}


void Integral::execute(const data::MIRField& field) {

    ASSERT(field.dimensions() == 1);
    ASSERT(!field.hasMissing());

    const repres::RepresentationHandle rep(field.representation());
    ASSERT(rep);

    const atlas::StructuredGrid structured(rep->atlasGrid());
    ASSERT(structured);

    integral_ = 0.;
    double weights = 0.;
    CounterUnary counter(field);

    auto& values = field.values(0);
    size_t i = 0;
    for (atlas::idx_t jlat = 0; jlat < structured.ny(); ++jlat) {

        auto pts_on_latitude = structured.nx(jlat);
        if (pts_on_latitude <= 0) {
            continue;
        }

        const double lat = structured.y(jlat);
        const double w = std::cos(util::degree_to_radian(lat)) / pts_on_latitude;

        for (atlas::idx_t jlon = 0; jlon < pts_on_latitude; ++jlon) {
            auto value = values[i++];
            if (!counter.missingValue(value)) {
                integral_ += w * value;
                weights += w;
            }
        }
    }
    ASSERT(counter.count() == values.size());

    integral_ /= weights;
}


void Integral::print(std::ostream& out) const {
    out << "Integral[" << integral_ << "]";
}


namespace {
static StatisticsBuilder<Integral> __stats("integral");
}


}  // namespace stats
}  // namespace mir
