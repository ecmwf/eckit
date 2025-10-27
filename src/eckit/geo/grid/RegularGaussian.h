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

#include "eckit/geo/grid/Regular.h"


namespace eckit::geo::grid {


class RegularGaussian final : public Regular {
public:

    // -- Constructors

    explicit RegularGaussian(const Spec&);
    explicit RegularGaussian(size_t N, area::BoundingBox = {}, Projection* = nullptr);

    // -- Methods

    size_t N() const { return N_; }

    // -- Overridden methods

    const std::string& type() const override;

    [[nodiscard]] Point first_point() const override;
    [[nodiscard]] Point last_point() const override;

    [[nodiscard]] Grid* make_grid_cropped(const Area&) const override;

    // -- Class members

    [[nodiscard]] static Spec* spec(const std::string& name);

private:

    // -- Members

    const size_t N_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::grid
