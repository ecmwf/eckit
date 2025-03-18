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

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/area/Polygon.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/log/JSON.h"
#include "eckit/parser/JSONParser.h"


namespace eckit::geo::area::library {


namespace {


ValueList list(const Value& j) {
    ASSERT(j.isList());
    return j.as<ValueList>();
}


polygon::Polygon polygon(const Value& j) {
    auto c = list(j);

    polygon::Polygon::container_type p;
    p.reserve(c.size());

    for (auto& l : c) {
        auto lonlat = list(l);

        ASSERT(lonlat.size() == 2);
        ASSERT(lonlat[0].isDouble() || lonlat[0].isNumber());
        ASSERT(lonlat[1].isDouble() || lonlat[1].isNumber());
        p.emplace_back(lonlat[0].as<double>(), lonlat[1].as<double>());
    }

    return polygon::Polygon(p);
}


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


GeoJSON::GeoJSON(const Spec& spec) : GeoJSON(PathName(spec.get_string("file")), spec.get_string("name", "")) {}


GeoJSON::GeoJSON(const PathName& file, const std::string& name) : GeoJSON(JSONParser::decodeFile(file), file, name) {}


GeoJSON::GeoJSON(const Value& json, const std::string& file, const std::string& name) : file_(file), name_(name) {
    build_polygons(json, polygons_);

    if (!name_.empty()) {
        ASSERT(json.isMap() && json["features"].isList());

        int _index = 0;
        for (const auto& feature : json["features"].as<ValueList>()) {
            ASSERT(feature.isMap() && feature["properties"].isMap() && feature["properties"][name].isString());

            auto _name = feature["properties"][name].as<std::string>();
            ASSERT(to_index.emplace(_name, _index++).second);
        }
    }
}


void GeoJSON::fill_spec(spec::Custom& custom) const {
    custom.set("type", "geojson");
    custom.set("file", file_.asString());
    if (!name_.empty()) {
        custom.set("name", name_);
    }
}


GeoJSON* GeoJSON::make_from_json_string(const std::string& json, const std::string& name) {
    return new GeoJSON(JSONParser::decodeString(json), "", name);
}


Area* GeoJSON::make_area(size_t index) const {
    return new area::Polygon(polygons_[index]);
}


Area* GeoJSON::make_area_from_name(const std::string& name) const {
    ASSERT(!name_.empty() && !name.empty());

    return make_area(to_index.at(name));
}


std::ostream& GeoJSON::list(std::ostream& out) const {
    JSON j(out);
    j.startObject();

    j << "type" << "geojson";
    if (!file_.asString().empty()) {
        j << "file" << file_;
    }
    j << "size" << size();
    if (!name_.empty()) {
        j << "name" << name_;
        j << "areas";
        j.startList();
        for (const auto& [key, val_] : to_index) {
            j << key;
        }
        j.endList();
    }

    j.endObject();
    return out;
}


}  // namespace eckit::geo::area::library
