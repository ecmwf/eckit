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

#include "eckit/parser/JSONParser.h"

#include "mir/geography/GeographyInput.h"


namespace eckit {
class PathName;
}


namespace mir {
namespace geography {


class GeoJSON : public GeographyInput {
public:
    // -- Types
    // None

    // -- Exceptions
    // None

    // -- Constructors

    explicit GeoJSON(const eckit::PathName&, Geometry);
    explicit GeoJSON(std::string& json, Geometry);

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    std::vector<Polygons> polygons() const override;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members
    // None

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

    eckit::Value json_;

    // -- Methods
    // None

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

    // -- Friends
    // None
};


}  // namespace geography
}  // namespace mir
