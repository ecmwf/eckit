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

#include <map>
#include <string>
#include <vector>

#include "eckit/geo/polygon/LonLatPolygon.h"
#include "eckit/value/Value.h"


namespace eckit {
class PathName;
namespace geo {
class Area;
class Spec;
namespace spec {
class Custom;
}
}  // namespace geo
}  // namespace eckit


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
    using Polygons = std::vector<polygon::LonLatPolygon>;

    // -- Constructors

    explicit GeoJSON(const Spec&);
    explicit GeoJSON(const PathName&, const std::string& name = "");

    // -- Methods

    std::ostream& list(std::ostream&) const;
    size_t size() const { return polygons_.size(); }

    [[nodiscard]] Area* make_area(size_t) const;
    [[nodiscard]] Area* make_area_from_name(const std::string&) const;

    // -- Class methods

    [[nodiscard]] static GeoJSON* make_from_json_string(const std::string& json, const std::string& name = "");

private:
    // -- Constructors

    explicit GeoJSON(const Value&, const std::string& file, const std::string& name);

    // -- Members

    const std::string file_;
    const std::string name_;

    std::map<std::string, int> to_index;
    std::vector<Polygons> polygons_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const;
};


}  // namespace eckit::geo::area::library
