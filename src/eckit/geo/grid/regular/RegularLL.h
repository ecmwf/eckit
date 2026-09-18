// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include "eckit/geo/grid/Regular.h"
#include "eckit/geo/range/Regular.h"


namespace eckit::geo::grid::regular {


class RegularLL : public Regular {
public:

    // -- Types

    struct Increments : std::array<double, 2> {
        Increments(value_type dlon, value_type dlat);

        using array::array;

        bool operator==(const Increments&) const;
        bool operator!=(const Increments& other) const { return !operator==(other); }

        value_type dlon() const { return operator[](0); }
        value_type dlat() const { return operator[](1); }

        static Increments make_from_spec(const Spec&);
    };

    using Reference = PointLonLat;

    // -- Constructors

    explicit RegularLL(const Spec&);
    explicit RegularLL(const Increments&, BoundingBox = {}, Reference = {}, order::Scan = scan_default(),
                       Projection* = nullptr);

    // -- Methods

    [[nodiscard]] static Spec* spec(const std::string& name);
    [[nodiscard]] PointLonLat reference() const { return {x_.shift(), y_.shift()}; }

    // -- Overridden methods

    const std::string& type() const override;

    [[nodiscard]] Point first_point() const override;
    [[nodiscard]] Point last_point() const override;

    [[nodiscard]] BoundingBox* calculate_bbox() const override;

    /// Bounding box in the grid's own (possibly rotated) frame
    [[nodiscard]] BoundingBox* calculate_bbox_xy() const;

    [[nodiscard]] Grid* make_grid_cropped(const Area&) const override;

    const Range& lon() const override { return x_; };
    const Range& lat() const override { return y_; };

    //-- Class methods

    [[nodiscard]] static Reference reference_default();

private:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

    // -- Members

    const range::RegularLongitude x_;
    const range::RegularLatitude y_;

    // -- Friends

    friend class geo::iterator::Regular;
};


}  // namespace eckit::geo::grid::regular
