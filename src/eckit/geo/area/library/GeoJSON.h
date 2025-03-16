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

#include "eckit/geo/polygon/LonLatPolygon.h"
#include "eckit/value/Value.h"


namespace eckit {
class PathName;
}


namespace eckit::geo::area::library {


class GeoJSON {
public:
    // -- Types

    /*
     * This Polygon definition maps to a single list of coordinates
     * Note:
     * - GeoJSON "Polygon" is a list of (this) Polygon (eg. "polygon"+"hole"+"hole"+...)
     * - GeoJSON "MultiPolygon" is a list of GeoJSON "Polygon"
     */
    using Polygon  = std::unique_ptr<polygon::LonLatPolygon>;
    using Polygons = std::vector<Polygon>;

    // -- Constructors

    explicit GeoJSON(const PathName&);
    explicit GeoJSON(std::string& json);

    // -- Methods

    std::ostream& list(std::ostream&) const;

    std::vector<Polygons> polygons() const;

private:
    // -- Members

    Value json_;
};


}  // namespace eckit::geo::area::library
