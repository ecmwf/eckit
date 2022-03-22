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


#include "mir/geography/GeoJSON.h"

#include "eckit/filesystem/PathName.h"

#include "mir/util/Atlas.h"
#include "mir/util/Exceptions.h"


namespace mir {
namespace geography {


namespace {


auto list = [](const eckit::Value& j) -> eckit::ValueList {
    ASSERT(j.isList());
    return j.as<eckit::ValueList>();
};


auto polygon = [](const eckit::Value& j,
                  const GeographyInput::Geometry& geom) -> GeographyInput::Polygon::element_type* {
    auto c = list(j);

    std::vector<atlas::PointLonLat> p;
    p.reserve(c.size());

    for (auto& l : c) {
        auto lonlat = list(l);

        ASSERT(lonlat.size() == 2);
        ASSERT(lonlat[0].isDouble() || lonlat[0].isNumber());
        ASSERT(lonlat[1].isDouble() || lonlat[1].isNumber());
        p.emplace_back(lonlat[0].as<double>(), lonlat[1].as<double>());
    }

    if (geom == GeographyInput::Geometry::LonLat) {
        return new atlas::util::PolygonXY(p);
    }

    if (geom == GeographyInput::Geometry::Spherical) {
        return new atlas::util::SphericalPolygon(p);
    }

    NOTIMP;
};


void build_polygons(const eckit::Value& j, const GeographyInput::Geometry& geom,
                    std::vector<GeographyInput::Polygons>& all) {
    if (j.isList()) {
        for (const auto& l : list(j)) {
            build_polygons(l, geom, all);
        }
        return;
    }

    if (j.isMap()) {
        if (j["type"].isString() && j["type"].as<std::string>() == "Polygon") {
            // "coordinates" is (a list) of (a list) of coordinates (lists of size 2)

            bool first = true;
            for (auto& lc : list(j["coordinates"])) {
                if (first) {
                    first = false;
                    all.emplace_back();
                }
                all.back().emplace_back(polygon(lc, geom));
            }
        }

        if (j["type"].isString() && j["type"].as<std::string>() == "MultiPolygon") {
            // "coordinates" is (a list) of (a list) of (a list) of coordinates (lists of size 2)

            bool first = true;
            for (auto& llc : list(j["coordinates"])) {
                for (auto& lc : list(llc)) {
                    if (first) {
                        first = false;
                        all.emplace_back();
                    }
                    all.back().emplace_back(polygon(lc, geom));
                }
            }
        }

        for (const auto& k : eckit::ValueMap(j)) {
            build_polygons(k.second, geom, all);
        }
        return;
    }
}


}  // namespace


GeoJSON::GeoJSON(const eckit::PathName& path, Geometry geometry) :
    GeographyInput(geometry), json_(eckit::JSONParser::decodeFile(path)) {}


GeoJSON::GeoJSON(std::string& json, Geometry geometry) :
    GeographyInput(geometry), json_(eckit::JSONParser::decodeString(json)) {}


std::vector<GeoJSON::Polygons> GeoJSON::polygons() const {
    std::vector<Polygons> p;
    build_polygons(json_, geometry(), p);
    return p;
}


}  // namespace geography
}  // namespace mir
