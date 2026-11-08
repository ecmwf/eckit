// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/area/library/GeoJSON.h"

#include <ostream>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/area/Polygon.h"
#include "eckit/log/JSON.h"
#include "eckit/parser/JSONParser.h"
#include "eckit/spec/Custom.h"
#include "eckit/spec/Spec.h"
#include "eckit/value/Content.h"


namespace eckit::geo::area::library {


namespace {


ValueList list(const Value& j) {
    if (!j.isList()) {
        throw exception::AreaError("GeoJSON: list expected", Here());
    }
    return j.as<ValueList>();
}


polygon::Polygon polygon(const Value& j) {
    auto c = list(j);

    polygon::Polygon::container_type p;
    p.reserve(c.size());

    for (auto& l : c) {
        // a position is [lon, lat], with an optional altitude (ignored)
        auto lonlat  = list(l);
        auto numeric = [](const Value& v) { return v.isDouble() || v.isNumber(); };
        if (lonlat.size() < 2 || !numeric(lonlat[0]) || !numeric(lonlat[1])) {
            throw exception::AreaError("GeoJSON: positions [lon, lat] expected", Here());
        }
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


GeoJSON::GeoJSON(const spec::Spec& spec) : GeoJSON(PathName(spec.get_string("file")), spec.get_string("name", "")) {}


GeoJSON::GeoJSON(const PathName& file, const std::string& name) : GeoJSON(JSONParser::decodeFile(file), file, name) {}


GeoJSON::GeoJSON(const Value& json, const std::string& file, const std::string& name) : file_(file), name_(name) {
    const auto features = json.isMap() && json["features"].isList();

    // of a feature collection, an area per feature (none for other geometries), so the areas index as the features
    if (features) {
        for (const auto& feature : json["features"].as<ValueList>()) {
            auto& area = polygons_.emplace_back();
            if (feature.isMap()) {
                std::vector<Polygons> found;
                build_polygons(feature["geometry"], found);
                for (auto& polygons : found) {
                    area.insert(area.end(), polygons.begin(), polygons.end());
                }
            }
        }
    }
    else {
        build_polygons(json, polygons_);
    }

    if (!name_.empty()) {
        if (!features) {
            throw exception::AreaError("GeoJSON: {features: [...]} expected, for areas by name", Here());
        }

        int index = 0;
        for (const auto& feature : json["features"].as<ValueList>()) {
            if (!(feature.isMap() && feature["properties"].isMap() && feature["properties"][name].isString())) {
                throw exception::AreaError("GeoJSON: features with a property '" + name + "' expected", Here());
            }

            const auto area = feature["properties"][name].as<std::string>();
            if (!to_index.emplace(area, index++).second) {
                throw exception::AreaError("GeoJSON: area '" + area + "' named more than once", Here());
            }
        }
    }
}


void GeoJSON::fill_spec(eckit::spec::Custom& custom) const {
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
    if (index >= polygons_.size()) {
        throw exception::AreaError("GeoJSON: area index " + std::to_string(index) + " beyond its " +
                                       std::to_string(polygons_.size()) + " areas",
                                   Here());
    }
    return new area::Polygon(polygons_[index]);
}


Area* GeoJSON::make_area_from_name(const std::string& name) const {
    auto it = name_.empty() ? to_index.end() : to_index.find(name);
    if (it == to_index.end()) {
        throw exception::AreaError("GeoJSON: unknown area '" + name + "'", Here());
    }

    return make_area(static_cast<size_t>(it->second));
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
