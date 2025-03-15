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
#include <string>
#include <vector>

#include "eckit/geometry/polygon/LonLatPolygon.h"


namespace mir {
namespace geography {


class GeographyInput {
public:
    // -- Types

    /*
     * This Polygon definition maps to a single list of coordinates
     * Note:
     * - GeoJSON "Polygon" is a list of (this) Polygon (eg. "polygon"+"hole"+"hole"+...)
     * - GeoJSON "MultiPolygon" is a list of GeoJSON "Polygon"
     */
    using Polygon  = std::unique_ptr<eckit::geometry::polygon::LonLatPolygon>;
    using Polygons = std::vector<Polygon>;

    enum Geometry
    {
        LonLat,
        Spherical
    };

    // -- Exceptions
    // None

    // -- Constructors
    // None

    // -- Destructor
    // None

    // -- Convertors
    // None

    // -- Operators
    // None

    // -- Methods

    virtual std::vector<Polygons> polygons() const = 0;
    static std::string geometryAsString(Geometry);
    static Geometry stringAsGeometry(const std::string&);

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

protected:
    // -- Members

    explicit GeographyInput(Geometry);

    // -- Methods

    Geometry geometry() const { return geometry_; }
    std::string geometryAsString() const;

    // -- Overridden methods
    // None

    // -- Class members
    // None

    // -- Class methods
    // None

private:
    // -- Members

    const Geometry geometry_;

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
