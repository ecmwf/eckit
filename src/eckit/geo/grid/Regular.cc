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

#include <algorithm>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/iterator/Regular.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/types/Fraction.h"


namespace eckit::geo::grid {


namespace {


area::BoundingBox* make_bounding_box(const Range& lon, const Range& lat) {
    auto n = std::max(lat.a(), lat.b());
    auto w = std::min(lon.a(), lon.b());
    auto s = std::min(lat.a(), lat.b());
    auto e = std::max(lon.a(), lon.b());
    return new area::BoundingBox{n, w, s, e};
}


}  // namespace


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


Regular::Regular(Ranges xy, Projection* projection) :
    Grid(make_bounding_box(*xy.first, *xy.second), projection), x_(xy.first), y_(xy.second) {
    ASSERT(x_ && x_->size() > 0);
    ASSERT(y_ && y_->size() > 0);
}


void Regular::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);
}


Renumber Regular::reorder(Ordering::ordering_type from, Ordering::ordering_type to, size_t nx, size_t ny) {
    ASSERT(0 < nx && 0 < ny);
    auto Ni = nx;
    auto Nj = ny;

    ASSERT(Ordering::scan_ordering <= from && from <= Ordering::scan_ordering_end &&
           not Ordering::is_scan_alternating_direction(from));

    ASSERT(Ordering::scan_ordering <= to && to <= Ordering::scan_ordering_end &&
           not Ordering::is_scan_alternating_direction(to));

    Renumber ren(nx * ny);

    if (from == Ordering::scan_i_positively_j_positively_ij_i_single_direction) {
        size_t count = 0;
        for (size_t j = Nj; j > 0; --j) {
            for (size_t i = 0; i < Ni; ++i) {
                ren[count++] = (j - 1) * Ni + i;
            }
        }

        return ren;
    }

    if (from == Ordering::scan_i_negatively_j_negatively_ij_i_single_direction) {
        size_t count = 0;
        for (size_t j = 0; j < Nj; ++j) {
            for (size_t i = Ni; i > 0; --i) {
                ren[count++] = j * Ni + (i - 1);
            }
        }

        return ren;
    }

    if (from == Ordering::scan_i_negatively_j_positively_ij_i_single_direction) {
        size_t count = 0;
        for (size_t j = Nj; j > 0; --j) {
            for (size_t i = Ni; i > 0; --i) {
                ren[count++] = (j - 1) * Ni + (i - 1);
            }
        }

        return ren;
    }

    throw exception::OrderingError("reorder(" + std::to_string(from) + ", " + std::to_string(to) + ")", Here());
}


Regular::Ranges::Ranges(Range* x, Range* y) : pair{x, y} {
    ASSERT(first != nullptr && second != nullptr);
}


}  // namespace eckit::geo::grid
