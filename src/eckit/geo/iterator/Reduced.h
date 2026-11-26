// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <vector>

#include "eckit/geo/Iterator.h"
#include "eckit/geo/Point.h"


namespace eckit::geo {
class Projection;
namespace grid {
class Reduced;
}
}  // namespace eckit::geo


namespace eckit::geo::iterator {


class Reduced : public geo::Iterator {
public:

    // -- Constructors

    explicit Reduced(const Grid&, size_t index = 0);

private:

    // -- Members

    const grid::Reduced& grid_;
    const Projection& projection_;

    std::vector<double> longitudes_j_;
    const std::vector<double>& latitudes_;
    const std::vector<size_t>& niacc_;
    const size_t size_;
    size_t j_;
    size_t index_;

    // -- Overridden operators

    bool operator==(const Iterator&) const override;
    bool operator++() override;
    bool operator+=(difference_type) override;
    explicit operator bool() const override;
    Point operator*() const override;

    // -- Overridden methods

    size_t index() const override { return index_; }
    size_t j(size_t idx) const;

    [[nodiscard]] Iterator* clone() const override { return new Reduced(*this); }
};


}  // namespace eckit::geo::iterator
