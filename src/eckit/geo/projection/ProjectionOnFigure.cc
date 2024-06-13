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

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/figure/Earth.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


ProjectionOnFigure::ProjectionOnFigure(const Spec&) : ProjectionOnFigure() {}


ProjectionOnFigure::ProjectionOnFigure(Figure* figure_ptr) :
    figure_(figure_ptr != nullptr ? figure_ptr : new figure::Earth) {
    ASSERT(figure_);
}


Figure* ProjectionOnFigure::make_figure() const {
    return FigureFactory::build(spec::Custom{{"a", figure_->a()}, {"b", figure_->b()}});
}


void ProjectionOnFigure::fill_spec(spec::Custom& custom) const {
    // FIXME OO figure
    if (types::is_approximately_equal(figure_->a(), figure_->b())) {
        custom.set("R", figure_->R());
        return;
    }
    custom.set("a", figure_->a());
    custom.set("b", figure_->b());
}


}  // namespace eckit::geo::projection
