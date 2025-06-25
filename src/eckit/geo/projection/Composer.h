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

    void fill_spec(spec::Custom&) const override;

    Point fwd(const Point&) const override;
    Point inv(const Point&) const override;

    // -- Class methods

    [[nodiscard]] static Projection* compose_back(Projection*, const Spec&);
    [[nodiscard]] static Projection* compose_front(const Spec&, Projection*);
};


}  // namespace eckit::geo::projection
