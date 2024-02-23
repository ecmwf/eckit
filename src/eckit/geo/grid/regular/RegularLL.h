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
#include "eckit/geo/range/RegularLatitude.h"
#include "eckit/geo/range/RegularLongitude.h"


namespace eckit::geo {
class Increments;
}  // namespace eckit::geo


namespace eckit::geo::grid::regular {


class RegularLL final : public Regular {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit RegularLL(const Spec&);
    explicit RegularLL(const Increments&, const area::BoundingBox& = {});
    RegularLL(const Increments&, const area::BoundingBox&, const PointLonLat& ref);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods
    // None

    // -- Overridden methods

    iterator cbegin() const override;
    iterator cend() const override;

    size_t ni() const override { return lon_.size(); }
    size_t nj() const override { return lat_.size(); }

    // -- Class members
    // None

    // -- Class methods

    static Spec* spec(const std::string& name);

private:
    // -- Members

    const range::RegularLongitude lon_;
    const range::RegularLatitude lat_;

    // -- Methods
    // None

    // -- Overridden methods

    const std::vector<double>& longitudes() const override { return lon_.values(); }
    const std::vector<double>& latitudes() const override { return lat_.values(); }

    void spec(spec::Custom&) const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::grid::regular
