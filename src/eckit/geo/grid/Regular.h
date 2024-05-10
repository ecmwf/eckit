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

#include <memory>
#include <utility>

#include "eckit/geo/Grid.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/Range.h"


namespace eckit::geo {
class Increments;
namespace iterator {
class Regular;
}
}  // namespace eckit::geo


namespace eckit::geo::grid {


class Regular : public Grid {
public:
    // -- Constructors

    explicit Regular(const Spec& spec) : Grid(spec) {}

    // -- Methods

    virtual double dx() const;
    virtual double dy() const;

    virtual size_t nx() const { return x_->size(); }
    virtual size_t ny() const { return y_->size(); }

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t size() const final { return nx() * ny(); }

    const Range& x() const;
    const Range& y() const;

protected:
    // -- Constructors

    Regular(std::pair<Range*, Range*> xy, const area::BoundingBox&);

    // -- Methods

    static std::pair<Range*, Range*> make_cartesian_ranges_from_spec(const Spec& spec);

private:
    // -- Members

    std::unique_ptr<Range> x_;
    std::unique_ptr<Range> y_;

    // -- Friends

    friend class geo::iterator::Regular;
};


struct RegularLL final : public Regular {
    explicit RegularLL(const Spec&);
    explicit RegularLL(const Increments&, const area::BoundingBox& = {});
    RegularLL(const Increments&, const area::BoundingBox&, const PointLonLat& ref);

    [[nodiscard]] static Spec* spec(const std::string& name);
    void spec(spec::Custom&) const override;
};


struct RegularGaussian final : public Regular {
    explicit RegularGaussian(const Spec&);
    explicit RegularGaussian(size_t N, const area::BoundingBox& = {});

    [[nodiscard]] Grid* make_grid_cropped(const area::BoundingBox& crop) const override;

    [[nodiscard]] static Spec* spec(const std::string& name);
    void spec(spec::Custom&) const override;

    size_t N() const { return N_; }

private:
    const size_t N_;
};


}  // namespace eckit::geo::grid
