// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <deque>
#include <vector>

#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


class Composer : public Projection, private std::deque<Projection*> {
public:

    // -- Constructors

    using deque::deque;
    explicit Composer() = default;

    Composer(const Composer&) = delete;
    Composer(Composer&&)      = delete;

    // -- Destructor

    ~Composer() override {
        for (auto* p : *this) {
            delete p;
        }
    }

    // -- Operators

    Composer& operator=(const Composer&) = delete;
    Composer& operator=(Composer&&)      = delete;

    // -- Methods

    using deque::clear;
    using deque::emplace_back;
    using deque::emplace_front;

    using deque::empty;
    using deque::size;

    std::vector<Point> fwd_points(const Point&) const;
    std::vector<Point> inv_points(const Point&) const;

    // -- Overridden methods

    const std::string& type() const override;

    Point fwd(const Point&) const override;
    Point inv(const Point&) const override;

    // -- Class methods

    [[nodiscard]] static Projection* compose_back(Projection*, const Spec&);
    [[nodiscard]] static Projection* compose_front(const Spec&, Projection*);

private:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::projection
