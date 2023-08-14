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

#include <vector>

#include "eckit/geometry/Grid.h"


namespace eckit::geometry::grid {


class HEALPix final : public Grid {
public:
    // -- Types

    enum ordering_type
    {
        ring,
        nested
    };

    // -- Exceptions
    // None

    // -- Constructors

    explicit HEALPix(const Configuration&);
    explicit HEALPix(size_t Nside, ordering_type);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members

    static Configuration* config(const std::string& name);

    // -- Class methods
    // None

private:
    // -- Members

    const size_t N_;
    const ordering_type ordering_;

    std::vector<size_t> njacc_;

    mutable std::vector<double> lats_;
    mutable std::vector<double> lons_;

    // -- Methods

    size_t ni() const;
    size_t nj(size_t i) const;

    const std::vector<double>& latitudes() const;
    const std::vector<double>& longitudes(size_t i) const;

    // -- Overridden methods

    const area::BoundingBox& boundingBox() const override;

    size_t size() const override;

    bool includesNorthPole() const override { return true; }
    bool includesSouthPole() const override { return true; }
    bool isPeriodicWestEast() const override { return true; }

    iterator cbegin() const override { NOTIMP; }
    iterator cend() const override { NOTIMP; }

    std::vector<Point>&& to_points() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geometry::grid
