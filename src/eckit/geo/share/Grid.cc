// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/share/Grid.h"

#include <algorithm>
#include <ostream>
#include <string>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/log/Log.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/spec/Custom.h"
#include "eckit/value/Content.h"
#include "eckit/value/Value.h"


namespace eckit::geo::share {


const Grid& Grid::instance() {
    static const Grid INSTANCE(LibEcKitGeo::shareGrid());
    return INSTANCE;
}


Grid::Grid(const std::vector<PathName>& paths) : spec_(new spec::Custom) {
    // a cached grid.yaml, unlike an installed one, can be invalid (rewritten when caching)
    const auto& roots = LibEcKitGeo::cacheRoots().paths();
    auto cached       = [&roots](const PathName& path) {
        return std::any_of(roots.begin(), roots.end(),
                                 [&path](const auto& root) { return path == root / "grid.yaml"; });
    };

    for (const auto& path : paths) {
        if (path.exists()) {
            Log::debug<LibEcKitGeo>() << "eckit::geo::share::Grid::load('" << path.realName() << "')" << std::endl;
            try {
                load(path);
            }
            catch (const eckit::Exception& e) {
                if (!cached(path)) {
                    throw;
                }
                Log::warning() << "Grid: '" << path << "' invalid, ignored (" << e.what() << ")" << std::endl;
            }
        }
    }
}


void Grid::load(const PathName& path) {
    auto* custom = dynamic_cast<spec::Custom*>(spec_.get());
    ASSERT(custom != nullptr);

    if (path.exists()) {
        ValueMap map(YAMLParser::decodeFile(path));

        for (const auto& kv : map) {
            const auto key = kv.first.as<std::string>();

            if (key == "grid_uids") {
                for (ValueMap m : kv.second.as<ValueList>()) {
                    if (m.size() != 1) {
                        throw exception::SpecError(
                            "Grid: '" + path.asString() + "' {grid_uids: [{uid: spec}, ...]} expected", Here());
                    }
                    const std::unique_ptr<spec::Custom> spec(spec::Custom::make_from_value(m.begin()->second));
                    GridSpecByUID::replace(m.begin()->first.as<std::string>(), *spec);
                }
                continue;
            }

            if (key == "grid_names") {
                for (ValueMap m : kv.second.as<ValueList>()) {
                    if (m.size() != 1) {
                        throw exception::SpecError(
                            "Grid: '" + path.asString() + "' {grid_names: [{name: spec}, ...]} expected", Here());
                    }
                    const std::unique_ptr<spec::Custom> spec(spec::Custom::make_from_value(m.begin()->second));
                    GridSpecByName::replace(m.begin()->first.as<std::string>(), *spec);
                }
                continue;
            }

            custom->set(key, kv.second);
        }
    }
}


}  // namespace eckit::geo::share
