// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <string>

#include "eckit/geo/Iterator.h"
#include "eckit/geo/Point.h"


namespace eckit::geo::grid {
class CubedSphere;
}


namespace eckit::geo::iterator {


class CubedSphere : public Iterator {
public:

    // -- Constructors

    CubedSphere(const grid::CubedSphere&, size_t index);

private:

    // -- Members

    const grid::CubedSphere& grid_;
    size_t index_;
    const size_t size_;
    const std::string uid_;

    // -- Overridden methods

    bool operator==(const geo::Iterator&) const override;
    bool operator++() override;
    bool operator+=(difference_type) override;

    explicit operator bool() const override;
    Point operator*() const override;

    size_t index() const override { return index_; }

    [[nodiscard]] Iterator* clone() const override { return new CubedSphere(*this); }
};


}  // namespace eckit::geo::iterator
