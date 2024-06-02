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


#include "eckit/geo/grid/Regular.h"

#include <regex>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Increments.h"
#include "eckit/geo/Shape.h"
#include "eckit/geo/Spec.h"
#include "eckit/geo/etc/Grid.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/range/RegularCartesian.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/types/Fraction.h"
#include "eckit/utils/Translator.h"


namespace eckit::geo::grid {


double Regular::dx() const {
    return x().increment();
}


double Regular::dy() const {
    return y().increment();
}


Grid::iterator Regular::cbegin() const {
    return iterator{new geo::iterator::Regular(*this, 0)};
}


Grid::iterator Regular::cend() const {
    return iterator{new geo::iterator::Regular(*this, size())};
}


const Range& Regular::x() const {
    ASSERT(x_ && x_->size() > 0);
    return *x_;
}


const Range& Regular::y() const {
    ASSERT(y_ && y_->size() > 0);
    return *y_;
}


Regular::Regular(std::pair<Range*, Range*> xy, const area::BoundingBox& bbox) :
    Grid(bbox), x_(xy.first), y_(xy.second) {
    ASSERT(x_ && x_->size() > 0);
    ASSERT(y_ && y_->size() > 0);
}


std::pair<Range*, Range*> Regular::make_cartesian_ranges_from_spec(const Spec& spec) {
    Increments inc(spec);
    Shape shape(spec);

    // FIXME This is a hack, we should not be using these keys
    Point2 a{spec.get_double("longitudeOfFirstGridPointInDegrees"),
             spec.get_double("latitudeOfFirstGridPointInDegrees")};
    Point2 b{a.X + inc.dx * static_cast<double>(shape.nx - 1), a.Y - inc.dy * static_cast<double>(shape.ny - 1)};

    return {new range::RegularCartesian(shape.nx, a.X, b.X), new range::RegularCartesian(shape.ny, a.Y, b.Y)};
}


void Regular::spec(spec::Custom& custom) const {
    Grid::spec(custom);

    custom.set("shape", std::vector<long>{static_cast<long>(nx()), static_cast<long>(ny())});
}


}  // namespace eckit::geo::grid
