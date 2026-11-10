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
        grid_(grid), projection_(grid.projection()), nxacc_(grid.nxacc()), latitudes_(grid.latitudes()) {}

    Point point(size_t index) override {
        const auto previous = j_;
        const auto c        = grid_.canonical(index, j_);

        if (longitudes_ == nullptr || previous != j_) {
            longitudes_ = &grid_.longitudes(j_);
        }

        return projection_.from_grid_xy(longitudes_->at(c - nxacc_[j_]), latitudes_.at(j_));
    }

    std::unique_ptr<Iterator::State> clone() const override { return std::make_unique<State>(*this); }

    const Reduced& grid_;
    const Projection& projection_;
    const std::vector<size_t>& nxacc_;
    const std::vector<double>& latitudes_;
    size_t j_                              = 0;
    const std::vector<double>* longitudes_ = nullptr;
};


Reduced::Reduced(order::ReducedScan scan, std::unique_ptr<const Projection> p) :
    Grid(nullptr, std::move(p)), scan_(std::move(scan)) {}


Point Reduced::point(size_t index) const {
    size_t j     = 0;
    const auto c = canonical(index, j);
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


size_t Reduced::canonical(size_t index, size_t& j) const {
    const auto& acc = nxacc();
    ASSERT(index < acc.back());

    // j+ reverses all the points, so rows are reversed for i+j+ and i-j-
    const auto j_positive = scan_.is_scan_j_positive();
    const auto c          = j_positive ? acc.back() - 1 - index : index;

    if (j + 1 >= acc.size() || c < acc[j] || acc[j + 1] <= c) {
        j = row(c);
    }

    const auto row_reversed = scan_.is_scan_i_positive() == j_positive;
    return row_reversed ? acc[j] + acc[j + 1] - 1 - c : c;
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
