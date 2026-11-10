// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/cache/Grid.h"

#include <fstream>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include "eckit/cache/DiskCache.h"
#include "eckit/cache/Roots.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/log/Log.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/spec/Custom.h"
#include "eckit/value/Content.h"


namespace eckit::geo::cache {


namespace {


struct lock_tag;  // of this file's mutex
using lock_type = util::lock_type<lock_tag>;


using section_t = std::map<std::string, std::string>;


/// Sections of grid.yaml (as share::Grid::load), empty if it doesn't exist or is invalid (so it is rewritten)
std::pair<section_t, section_t> read(const PathName& path) {
    if (!path.exists()) {
        return {};
    }

    auto read_section = [&path](const Value& value, const std::string& name) {
        section_t section;
        for (ValueMap m : value.as<ValueList>()) {
            if (m.size() != 1) {
                throw exception::SpecError("Grid: '" + path + "' {" + name + ": [{key: spec}, ...]} expected", Here());
            }
            std::unique_ptr<const geo::Grid::Spec> spec(spec::Custom::make_from_value(m.begin()->second));
            section.emplace(m.begin()->first.as<std::string>(), spec->str());
        }
        return section;
    };

    try {
        section_t uids;
        section_t names;

        for (const auto& [key, value] : ValueMap(YAMLParser::decodeFile(path))) {
            if (const auto k = key.as<std::string>(); k == "grid_uids") {
                uids = read_section(value, k);
            }
            else if (k == "grid_names") {
                names = read_section(value, k);
            }
        }

        return {uids, names};
    }
    catch (const eckit::Exception& e) {
        Log::warning() << "Grid: '" << path << "' invalid, rewritten (" << e.what() << ")" << std::endl;
        return {};
    }
}


void write(const PathName& path, const section_t& uids, const section_t& names) {
    std::ofstream fs(path.asString());

    // keys quoted (names can have any characters), values as JSON (as YAML)
    auto quoted = [](const std::string& str) {
        std::string q{'"'};
        for (auto c : str) {
            if (c == '"' || c == '\\') {
                q += '\\';
            }
            q += c;
        }
        return q + '"';
    };

    auto write_section = [&fs, &quoted](const std::string& name, const section_t& section) {
        if (!section.empty()) {
            fs << '\n' << name << ':' << '\n';
            for (const auto& [key, value] : section) {
                fs << "  - " << quoted(key) << ": " << value << '\n';
            }
        }
    };

    fs << "---\n";
    write_section("grid_names", names);
    write_section("grid_uids", uids);

    fs.close();
    if (!fs) {
        throw WriteError("Grid: cannot write '" + path + "'", Here());
    }
}


}  // namespace


Grid& Grid::instance() {
    static Grid INSTANCE;
    return INSTANCE;
}


void Grid::save(const geo::Grid::uid_type& uid, const geo::Grid::Spec& spec) {
    lock_type lock;

    // nowhere to write (not caching, or a read-only cache)
    if (LibEcKitGeo::cacheWritablePaths().empty()) {
        return;
    }

    const auto spec_str = spec.str();
    auto name           = spec.get_string("name", "");
    if (geo::Grid::is_uid(name)) {
        name.clear();
    }

    auto has = [&spec_str](const section_t& section, const std::string& key) {
        auto it = section.find(key);
        return it != section.end() && it->second == spec_str;
    };

    const auto path = LibEcKitGeo::cacheGrid();
    eckit::cache::Roots::create_directory(path.dirName());

    if (auto [uids, names] = read(path); has(uids, uid) && (name.empty() || has(names, name))) {
        return;
    }

    Log::debug<LibEcKitGeo>() << "eckit::geo::cache::Grid::save(uid='" + uid + "'"
                              << (name.empty() ? "" : ", name='" + name + "'") << ")" << std::endl;

    // across processes, read again (another process can have written it meanwhile)
    eckit::cache::DiskCache::write(path, [&](const PathName& tmp) {
        auto [uids, names] = read(path);

        uids[uid] = spec_str;
        if (!name.empty()) {
            names[name] = spec_str;
        }

        write(tmp, uids, names);
    });
}


}  // namespace eckit::geo::cache
