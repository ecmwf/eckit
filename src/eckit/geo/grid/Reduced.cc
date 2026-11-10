// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/Reduced.h"

#include <algorithm>
#include <iterator>
#include <utility>

#include "eckit/geo/Exceptions.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo::grid {


/// The current row and its longitudes (rows in the default order)
struct Reduced::State final : Iterator::State {
    explicit State(const Reduced& grid) :
        grid_(grid),
        projection_(grid.projection()),
        canonical_(grid.scan_.is_default() ? nullptr : &grid.canonical()),
        nxacc_(grid.nxacc()),
        latitudes_(grid.latitudes()) {}

    Point point(size_t index) override {
        // the point as in the default order
        const auto c = canonical_ == nullptr ? index : canonical_->at(index);

        if (longitudes_ == nullptr || !(nxacc_[j_] <= c && c < nxacc_[j_ + 1])) {
            longitudes_ = &grid_.longitudes(j_ = grid_.row(c));
        }

        return projection_.from_grid_xy(longitudes_->at(c - nxacc_[j_]), latitudes_.at(j_));
    }

    std::unique_ptr<Iterator::State> clone() const override { return std::make_unique<State>(*this); }

    const Reduced& grid_;
    const Projection& projection_;
    const renumber_type* canonical_;
    const std::vector<size_t>& nxacc_;
    const std::vector<double>& latitudes_;
    size_t j_                              = 0;
    const std::vector<double>* longitudes_ = nullptr;
};


Reduced::Reduced(order::ReducedScan scan, std::unique_ptr<const Projection> p) :
    Grid(nullptr, std::move(p)), scan_(std::move(scan)) {}


Point Reduced::point(size_t index) const {
    const auto c = scan_.is_default() ? index : canonical().at(index);
    const auto j = row(c);
    return projection().from_grid_xy(longitude(j, c - nxacc()[j]), latitudes().at(j));
}


std::unique_ptr<Grid::Iterator::State> Reduced::make_state() const {
    return std::make_unique<State>(*this);
}


void Reduced::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);

    if (!scan_.is_default()) {
        custom.set("order", scan_.order());
    }
}


pl_type Reduced::points_per_row() const {
    pl_type pl(ny());
    for (size_t j = 0; j < pl.size(); ++j) {
        pl[j] = static_cast<pl_type::value_type>(nxj(j));
    }
    return pl;
}


const Grid::renumber_type& Reduced::canonical() const {
    util::call_once(canonical_once_, [this]() { canonical_ = scan_.canonical(points_per_row()); });
    return canonical_;
}


const std::vector<size_t>& Reduced::nxacc() const {
    util::call_once(nxacc_once_, [this]() {
        nxacc_.resize(1 + ny());
        nxacc_.front() = 0;

        for (size_t j = 0; j < ny(); ++j) {
            nxacc_[j + 1] = nxacc_[j] + nxj(j);
        }
    });

    return nxacc_;
}


size_t Reduced::row(size_t index) const {
    const auto& acc = nxacc();
    ASSERT(index < acc.back());

    return static_cast<size_t>(std::distance(acc.begin(), std::upper_bound(acc.begin(), acc.end(), index))) - 1;
}


}  // namespace eckit::geo::grid
