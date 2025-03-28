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

#include "eckit/geo/Projection.h"


namespace eckit::geo::projection {


class XYToLonLat : public Projection {
public:

    // -- Constructors

    explicit XYToLonLat() = default;
    explicit XYToLonLat(const Spec&) {}

    // -- Methods

    const std::string& type() const override;

    inline PointLonLat fwd(const PointXY& p) const { return {p.X, p.Y}; }
    inline PointXY inv(const PointLonLat& q) const { return {q.lon, q.lat}; }

    // -- Overridden methods

    inline Point fwd(const Point& p) const override { return fwd(std::get<PointXY>(p)); }
    inline Point inv(const Point& q) const override { return inv(std::get<PointLonLat>(q)); }

protected:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
};


using PlateCaree = XYToLonLat;


}  // namespace eckit::geo::projection
