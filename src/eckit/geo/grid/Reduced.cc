// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/Reduced.h"

#include <algorithm>
#include <iterator>

#include "eckit/geo/Exceptions.h"


namespace eckit::geo::grid {


/// The current row and its longitudes
struct Reduced::State final : Iterator::State {
    explicit State(const Reduced& grid) :
        grid_(grid), projection_(grid.projection()), nxacc_(grid.nxacc()), latitudes_(grid.latitudes()) {}

    Point point(size_t index) override {
        if (longitudes_.empty() || !(nxacc_[j_] <= index && index < nxacc_[j_ + 1])) {
            longitudes_ = grid_.longitudes(j_ = grid_.row(index));
        }

        return projection_.from_grid_xy(longitudes_.at(index - nxacc_[j_]), latitudes_.at(j_));
    }

    [[nodiscard]] Iterator::State* clone() const override { return new State(*this); }

    const Reduced& grid_;
    const Projection& projection_;
    const std::vector<size_t>& nxacc_;
    const std::vector<double>& latitudes_;
    size_t j_ = 0;
    std::vector<double> longitudes_;
};


Point Reduced::point(size_t index) const {
    return State(*this).point(index);
}


Grid::Iterator::State* Reduced::make_state() const {
    return new State(*this);
}


Point Reduced::first_point() const {
    ASSERT(!empty());
    return PointLonLat{longitudes(0).front(), latitudes().front()};
}


Point Reduced::last_point() const {
    ASSERT(!empty());

    auto j = static_cast<int>(ny()) - 1;
    ASSERT(0 <= j);

    return PointLonLat{longitudes(j).back(), latitudes().back()};
}


const std::vector<size_t>& Reduced::nxacc() const {
    if (nxacc_.empty()) {
        nxacc_.resize(1 + ny());
        nxacc_.front() = 0;

        size_t j = 0;
        for (auto a = nxacc_.begin(), b = a + 1; b != nxacc_.end(); ++j, ++a, ++b) {
            *b = *a + nxj(j);
        }

        ASSERT(nxacc_.back() == size());
    }

    return nxacc_;
}


size_t Reduced::row(size_t index) const {
    const auto& acc = nxacc();
    ASSERT(index < acc.back());

    return static_cast<size_t>(std::distance(acc.begin(), std::upper_bound(acc.begin(), acc.end(), index))) - 1;
}


}  // namespace eckit::geo::grid
