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
#include "eckit/geo/range/Regular.h"


namespace eckit::geo::grid::regular {


class RegularLL final : public Regular {
public:

    // -- Types

    struct Increments : public std::array<double, 2> {
        Increments(value_type dlon, value_type dlat) : array{dlon, dlat} {}

        using array::array;

        bool operator==(const Increments&) const;
        bool operator!=(const Increments& other) const { return !operator==(other); }

        const value_type& dlon = array::operator[](0);
        const value_type& dlat = array::operator[](1);
    };

    // -- Constructors

    explicit RegularLL(const Spec&);
    explicit RegularLL(const Increments&);
    explicit RegularLL(const Increments&, BoundingBox, PointLonLat ref = {});

    // -- Methods

    [[nodiscard]] static Spec* spec(const std::string& name);
    [[nodiscard]] PointLonLat reference() const { return {x_.shift(), y_.shift()}; }

    double dlon() const { return dx(); }
    double dlat() const { return dy(); }

    size_t nlon() const { return nx(); }
    size_t nlat() const { return ny(); }

    // -- Overridden methods

    const std::string& type() const override;

    [[nodiscard]] Point first_point() const override;
    [[nodiscard]] Point last_point() const override;
    [[nodiscard]] std::pair<std::vector<double>, std::vector<double>> to_latlons() const override;

    [[nodiscard]] Grid* make_grid_cropped(const Area&) const override;
    [[nodiscard]] BoundingBox* calculate_bbox() const override;
    double dx() const override { return x_.increment(); }
    double dy() const override { return y_.increment(); }

    size_t nx() const override { return x_.size(); }
    size_t ny() const override { return y_.size(); }

    const Range& x() const override { return x_; }
    const Range& y() const override { return y_; }


    // -- Class methods

    static Increments make_increments_from_spec(const Spec&);

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
