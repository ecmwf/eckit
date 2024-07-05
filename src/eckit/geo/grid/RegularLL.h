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


class RegularLL final : public Regular {
public:
    // -- Constructors

    explicit RegularLL(const Spec&);
    explicit RegularLL(const Increments&, const area::BoundingBox& = {}, projection::Rotation* = nullptr);
    RegularLL(const Increments&, const area::BoundingBox&, projection::Rotation*, const PointLonLat& ref);

    // -- Methods

    [[nodiscard]] static Spec* spec(const std::string& name);

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

    [[nodiscard]] Grid* make_grid_cropped(const Area&) const override;
    [[nodiscard]] area::BoundingBox* calculate_bbox() const override;
};


}  // namespace eckit::geo::grid
