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


#include "eckit/geo/cache/Grid.h"

#include <fstream>
#include <map>
#include <memory>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/log/Log.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo::cache {


static util::recursive_mutex MUTEX;


Grid& Grid::instance() {
    static Grid INSTANCE;
    return INSTANCE;
}


void Grid::save(const geo::Grid::uid_type& uid, const geo::Grid::Spec& spec) {
    util::lock_guard<util::recursive_mutex> lock(MUTEX);

    if (!LibEcKitGeo::caching()) {
        return;
    }

    // TODO implement proper serialise/unserialise
    // NOTE this code is in sync with share::Grid::load

    using section_t = std::map<std::string, std::string>;
    section_t grid_uids;
    section_t grid_names;

    const auto path = LibEcKitGeo::cacheGrid();
    if (path.exists()) {
        ValueMap map(YAMLParser::decodeFile(path));

        for (const auto& kv : map) {
            const auto key = kv.first.as<std::string>();

            if (key == "grid_uids") {
                for (ValueMap m : kv.second.as<ValueList>()) {
                    ASSERT(m.size() == 1);
                    std::unique_ptr<const geo::Grid::Spec> spec(spec::Custom::make_from_value(m.begin()->second));
                    grid_uids.emplace(m.begin()->first.as<std::string>(), spec->str());
                }
                continue;
            }

            if (key == "grid_names") {
                for (ValueMap m : kv.second.as<ValueList>()) {
                    ASSERT(m.size() == 1);
                    std::unique_ptr<const geo::Grid::Spec> spec(spec::Custom::make_from_value(m.begin()->second));
                    grid_names.emplace(m.begin()->first.as<std::string>(), spec->str());
                }
                continue;
            }
        }
    }


    auto spec_str = spec.str();
    auto name     = spec.get_string("name", "");
    if (geo::Grid::is_uid(name)) {
        name.clear();
    }

    // avoid writing if there's no new information
    if (auto not_new =
            [](const section_t& section, const std::string& key, const std::string& value) {
                auto it = section.find(key);
                return it != section.end() && it->second == value;
            };
        not_new(grid_uids, uid, spec_str) && (name.empty() || not_new(grid_names, name, spec_str))) {
        return;
    }

    Log::debug<LibEcKitGeo>() << "eckit::geo::cache::Grid::regist(uid='" + uid + "'"
                              << (name.empty() ? "" : ", name='" + name + "'") << ")" << std::endl;

    std::ofstream fs(path);
    ASSERT(fs);

    auto write_section = [&fs](const std::string& name, const section_t& section) {
        if (!section.empty()) {
            fs << '\n' << name << ':' << '\n';
            for (const auto& [key, value] : section) {
                fs << "  - " << key << ": " << value << '\n';
            }
        }
    };

    grid_uids[uid] = spec_str;
    if (!name.empty()) {
        grid_names[name] = spec_str;
    }

    fs << "---\n";
    write_section("grid_names", grid_names);
    write_section("grid_uids", grid_uids);
}


}  // namespace eckit::geo::cache
