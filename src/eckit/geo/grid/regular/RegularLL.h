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

#include "eckit/geo/grid/Regular.h"


namespace eckit::geo {
class Increments;
class Range;
}  // namespace eckit::geo


namespace eckit::geo::grid::regular {


class RegularLL final : public Regular {
private:
    // -- Types

    struct Internal {
        Internal(const Increments&, const area::BoundingBox&, const PointLonLat& _ref);

        Increments inc;
        area::BoundingBox bbox;
        PointLonLat first;

        size_t ni = 0;
        size_t nj = 0;
    };

    // -- Constructors

    explicit RegularLL(Internal&&);

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

    size_t ni() const override { return internal_.ni; }
    size_t nj() const override { return internal_.nj; }

    // -- Class members
    // None

    // -- Class methods

    static Spec* config(const std::string& name);

private:
    // -- Members

    const Internal internal_;
    const std::unique_ptr<Range> range_longitude_;
    const std::unique_ptr<Range> range_latitude_;

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
