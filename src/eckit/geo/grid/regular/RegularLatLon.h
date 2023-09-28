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


namespace eckit::geo {
class Increments;
}


namespace eckit::geo::grid::regular {


class RegularLatLon final : public Regular {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit RegularLatLon(const Configuration&);

    RegularLatLon(const Increments&, const area::BoundingBox&, const PointLonLat& reference = {0, 0});
    RegularLatLon(size_t ni, size_t nj, const area::BoundingBox&, const PointLonLat& reference = {0, 0});

    explicit RegularLatLon(const Increments&, const PointLonLat& reference = {0, 0});
    RegularLatLon(size_t ni, size_t nj, const PointLonLat& reference = {0, 0});

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

    size_t ni() const override { return ni_; }
    size_t nj() const override { return nj_; }

    // -- Class members
    // None

    // -- Class methods

    static Configuration* config(const std::string& name);

private:
    // -- Members

    size_t ni_;
    size_t nj_;

    // -- Methods
    // None

    // -- Overridden methods

    const std::vector<double>& longitudes() const override;
    const std::vector<double>& latitudes() const override;

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace eckit::geo::grid::regular
