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

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/area/Library.h"
#include "eckit/geo/polygon/Polygon.h"
#include "eckit/value/Value.h"


namespace eckit::geo {
class Area;
class Spec;
namespace spec {
class Custom;
}
}  // namespace eckit::geo


namespace eckit::geo::area::library {


class GeoJSON : public Library {
public:

    // -- Types

    /*
     * Note:
     * - geo::polygon::Polygon is a list of (geo::PointLonLat) points
     * - GeoJSON "Polygon" is a list of (geo::polygon::Polygon) polygon (eg. "polygon"+"hole"+"hole"+...)
     * - GeoJSON "MultiPolygon" is a list of GeoJSON "Polygon"
     */
    using Polygons = std::vector<polygon::Polygon>;

    // -- Constructors

    explicit GeoJSON(const Spec&);
    explicit GeoJSON(const PathName&, const std::string& name = "");

    // -- Overridden methods

    std::ostream& list(std::ostream&) const override;
    size_t size() const override { return polygons_.size(); }

    [[nodiscard]] Area* make_area(size_t) const override;
    [[nodiscard]] Area* make_area_from_name(const std::string&) const override;

    // -- Class methods

    [[nodiscard]] static GeoJSON* make_from_json_string(const std::string& json, const std::string& name = "");

private:

    // -- Constructors

    explicit GeoJSON(const Value&, const std::string& file, const std::string& name);

    // -- Members

    const PathName file_;
    const std::string name_;

    std::map<std::string, int> to_index;
    std::vector<Polygons> polygons_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::area::library
