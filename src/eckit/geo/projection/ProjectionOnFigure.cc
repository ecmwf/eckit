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


#include "eckit/geo/projection/ProjectionOnFigure.h"

#include <cmath>

#include "eckit/geo/geometry/Earth.h"
#include "eckit/geo/util.h"


namespace eckit::geo::projection {


ProjectionOnFigure::sincos_t::sincos_t(value_type r) : array{std::sin(r), std::sqrt(1. - std::sin(r) * std::sin(r))} {}


ProjectionOnFigure::PointLonLatR::PointLonLatR(value_type lonr, value_type latr) : array{lonr, latr} {}


ProjectionOnFigure::PointLonLatR::PointLonLatR(const PointLonLat& p) :
    PointLonLatR{p.lon * util::DEGREE_TO_RADIAN, p.lat * util::DEGREE_TO_RADIAN} {}


// FIXME refactor figures into a single hierarchy
struct Earth final : Figure {
    Earth() = default;

    double R() const override { return geometry::Earth::radius(); }
    double a() const override { return geometry::Earth::radius(); }
    double b() const override { return geometry::Earth::radius(); }
};


ProjectionOnFigure::ProjectionOnFigure(const Spec&) : ProjectionOnFigure() {}


ProjectionOnFigure::ProjectionOnFigure(Figure* figure_ptr) : figure_(figure_ptr != nullptr ? figure_ptr : new Earth) {}


}  // namespace eckit::geo::projection
