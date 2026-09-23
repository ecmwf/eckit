// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>

#include "eckit/geo/Iterator.h"


namespace eckit::geo {
class Projection;
namespace grid {
class Regular;
}
}  // namespace eckit::geo


namespace eckit::geo::iterator {


class Regular : public Iterator {
public:

    // -- Constructors

    explicit Regular(const grid::Regular&, size_t index = 0);

private:

    // -- Members

    const Projection& projection_;

    const bool xy_;
    const std::vector<double>& x_;
    const std::vector<double>& y_;
    size_t ix_;
    size_t iy_;
    size_t index_;

    const size_t nx_;
    const size_t ny_;
    const size_t size_;

    // -- Overridden methods

    bool operator==(const Iterator&) const override;
    bool operator++() override;
    bool operator+=(difference_type) override;
    explicit operator bool() const override;
    Point operator*() const override;

    size_t index() const override { return index_; }

    [[nodiscard]] Iterator* clone() const override { return new Regular(*this); }
};


}  // namespace eckit::geo::iterator
