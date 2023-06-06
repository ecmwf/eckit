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

#include "eckit/geo/grid/RegularGrid.h"


namespace eckit::geo::grid {
namespace detail {


/*
 * References:
 * - [1] LRIT/HRIT Global Specification (CGMS 03, Issue 2.6, 12.08.1999)
 * - [2] MSG Ground Segment LRIT/HRIT Mission Specific Implementation, EUMETSAT Document, (EUM/MSG/SPE/057, Issue 6, 21.
 * June 2006)
 * - [3] MSG Ground Segment LRIT/HRIT Mission Specific Implementation, EUMETSAT Document, (EUM/MSG/SPE/057 v7 e-signed.
 * 30 November 2015)
 */
struct SpaceViewInternal {
    SpaceViewInternal(const Configuration&);
    Projection projection_;
    Projection projectionGreenwich_;
    BoundingBox bbox_;

    RegularGrid::LinearSpacing x() const { return {xa_, xb_, Nx_, true}; }
    RegularGrid::LinearSpacing y() const { return {ya_, yb_, Ny_, true}; }

    const std::vector<RegularGrid::PointLonLat>& lonlat() const;

    long Nx_;
    long Ny_;
    double xa_;
    double xb_;
    double ya_;
    double yb_;
    double LongestElementDiagonal_;
    double Lop_;

private:
    mutable std::vector<RegularGrid::PointLonLat> lonlat_;
};


}  // namespace detail


class SpaceView final : public detail::SpaceViewInternal, public RegularGrid {
public:
    // -- Exceptions
    // None

    // -- Constructors

    SpaceView(const Configuration&);
    SpaceView(const SpaceView&) = delete;

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators

    SpaceView& operator=(const SpaceView&) = delete;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members
    // None

    // -- Methods
    // None

    // -- Overridden methods

    Iterator* iterator() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::grid
