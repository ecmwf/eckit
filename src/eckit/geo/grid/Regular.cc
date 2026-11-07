// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/Regular.h"

#include <algorithm>
#include <cmath>
#include <memory>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/Range.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::grid {


namespace {


/// Indices of the values of a (cropped) range in the (monotonic) values of a range
std::vector<size_t> indices_of(const Range& cropped, const Range& range) {
    const auto& values    = range.values();
    const auto increasing = values.front() <= values.back();
    const auto min        = std::min(values.front(), values.back()) - PointLonLat::EPS;

    std::vector<size_t> indices;
    indices.reserve(cropped.size());

    for (auto value : cropped.values()) {
        if (range.periodic()) {
            value = PointLonLat::normalise_angle_to_minimum(value, min);
        }

        // the nearest value
        auto it = std::lower_bound(values.begin(), values.end(), value,
                                   [increasing](double a, double b) { return increasing ? a < b : a > b; });
        if (it == values.end() || (it != values.begin() && std::abs(*(it - 1) - value) < std::abs(*it - value))) {
            --it;
        }

        ASSERT(types::is_approximately_equal(*it, value, PointLonLat::EPS));
        indices.push_back(static_cast<size_t>(it - values.begin()));
    }

    return indices;
}


/// Any of the values of a range within [a, b]
bool any_within(const Range& range, double a, double b) {
    const auto& values = range.values();
    return std::any_of(values.begin(), values.end(), [a, b, periodic = range.periodic()](double value) {
        if (periodic) {
            value = PointLonLat::normalise_angle_to_minimum(value, a - PointLonLat::EPS);
        }
        return types::is_approximately_greater_or_equal(value, a, PointLonLat::EPS) &&
               types::is_approximately_lesser_or_equal(value, b, PointLonLat::EPS);
    });
}


}  // namespace


/// The coordinates by scanning direction: along (a, as rows or columns) and across (b)
struct Regular::State final : Iterator::State {
    explicit State(const Regular& grid) :
        grid_(grid),
        i_then_j_(grid.scan_.is_scan_i_then_j()),
        alternating_(grid.scan_.is_scan_alternating()),
        a_((i_then_j_ ? grid.x() : grid.y()).values()),
        b_((i_then_j_ ? grid.y() : grid.x()).values()) {}

    Point point(size_t index) override {
        const auto j = index / a_.size();
        const auto i = index % a_.size();
        const auto k = alternating_ && j % 2 == 1 ? a_.size() - 1 - i : i;
        return i_then_j_ ? grid_.point_xy(a_.at(k), b_.at(j)) : grid_.point_xy(b_.at(j), a_.at(k));
    }

    [[nodiscard]] Iterator::State* clone() const override { return new State(*this); }

    const Regular& grid_;
    const bool i_then_j_;
    const bool alternating_;
    const std::vector<double>& a_;
    const std::vector<double>& b_;
};


Point Regular::point(size_t index) const {
    return State(*this).point(index);
}


Grid::Iterator::State* Regular::make_state() const {
    return new State(*this);
}


const Regular::order_type& Regular::order() const {
    return scan_.order();
}


Grid::renumber_type Regular::reorder(const order_type& to) const {
    return scan_.reorder(to, nx(), ny());
}


Grid* Regular::make_grid_cropped(const Area& area) const {
    auto* cropped = regular_cropped(area);
    return cropped != nullptr ? cropped : make_grid_cropped_unstructured_ll(area);
}


Regular* Regular::regular_cropped(const Area& area) const {
    // by a bounding box (unprojected) with grid points
    auto bbox = boundingBox();
    return projection().is_default() && dynamic_cast<const area::BoundingBox*>(&area) != nullptr &&
                   area.intersects(bbox) && any_within(x(), bbox.west(), bbox.east()) &&
                   any_within(y(), bbox.south(), bbox.north())
               ? make_regular_cropped(bbox)
               : nullptr;
}


Grid::renumber_type Regular::crop(const Area& area) const {
    std::unique_ptr<const Regular> cropped(regular_cropped(area));
    if (!cropped) {
        return Grid::crop(area);
    }

    // the cropped grid points (by its x, y values) as these grid points, in the cropped grid order
    const auto i = indices_of(cropped->x(), x());
    const auto j = indices_of(cropped->y(), y());

    renumber_type indices(cropped->size());
    for (size_t cj = 0; cj < j.size(); ++cj) {
        for (size_t ci = 0; ci < i.size(); ++ci) {
            indices[cropped->index(ci, cj)] = index(i[ci], j[cj]);
        }
    }

    return indices;
}


size_t Regular::index(size_t i, size_t j) const {
    ASSERT(i < nx() && j < ny());

    // rows (i then j) or columns (j then i), the odd ones reversed if scanning in alternating directions
    const auto alternating = scan_.is_scan_alternating();
    return scan_.is_scan_i_then_j() ? j * nx() + (alternating && j % 2 == 1 ? nx() - 1 - i : i)
                                    : i * ny() + (alternating && i % 2 == 1 ? ny() - 1 - j : j);
}


const order::RegularScan& Regular::scan_default() {
    static const order::RegularScan SCAN;
    return SCAN;
}


Regular::Regular(order::RegularScan s, const Projection* proj) : Grid(nullptr, proj), scan_(s) {}


void Regular::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);

    if (scan_.order() != order::RegularScan::order_default()) {
        custom.set("order", scan_.order());
    }
}

void Regular::scan(const order_type& to) {
    scan_.order(to);
}

void Regular::scan(order::RegularScan s) {
    scan_ = s;
}


}  // namespace eckit::geo::grid
