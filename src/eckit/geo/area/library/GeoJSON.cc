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


#include "eckit/geo/area/library/GeoJSON.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Point.h"
#include "eckit/parser/JSONParser.h"


namespace eckit::geo::area::library {


namespace {


const auto list = [](const Value& j) -> ValueList {
    ASSERT(j.isList());
    return j.as<ValueList>();
};


const auto polygon = [](const Value& j) -> GeoJSON::Polygon::element_type* {
    auto c = list(j);

    std::vector<Point2> p;
    p.reserve(c.size());

    for (auto& l : c) {
        auto lonlat = list(l);

        ASSERT(lonlat.size() == 2);
        ASSERT(lonlat[0].isDouble() || lonlat[0].isNumber());
        ASSERT(lonlat[1].isDouble() || lonlat[1].isNumber());
        p.emplace_back(lonlat[0].as<double>(), lonlat[1].as<double>());
    }

    // return new GeographyInput::Polygon::element_type(p);
    // return new atlas::util::SphericalPolygon(p);

    NOTIMP;
};


void build_polygons(const Value& j, std::vector<GeoJSON::Polygons>& all) {
    if (j.isList()) {
        for (const auto& l : list(j)) {
            build_polygons(l, all);
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
                all.back().emplace_back(polygon(lc));
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
                    all.back().emplace_back(polygon(lc));
                }
            }
        }

        for (const auto& k : ValueMap(j)) {
            build_polygons(k.second, all);
        }
        return;
    }
}


}  // namespace


GeoJSON::GeoJSON(const PathName& path) : json_(JSONParser::decodeFile(path)) {}


GeoJSON::GeoJSON(std::string& json) : json_(JSONParser::decodeString(json)) {}


std::ostream& GeoJSON::list(std::ostream&) const {
    NOTIMP;
}


std::vector<GeoJSON::Polygons> GeoJSON::polygons() const {
    std::vector<Polygons> p;
    build_polygons(json_, p);
    return p;
}


}  // namespace eckit::geo::area::library
