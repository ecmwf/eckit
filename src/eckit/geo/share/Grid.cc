// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/share/Grid.h"

#include <memory>
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
    ASSERT(spec_);

    for (const auto& path : paths) {
        if (path.exists()) {
            Log::debug<LibEcKitGeo>() << "eckit::geo::share::Grid::load('" << path.realName() << "')" << std::endl;
            load(path);
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
                    ASSERT(m.size() == 1);
                    const std::unique_ptr<spec::Custom> spec(spec::Custom::make_from_value(m.begin()->second));
                    GridSpecByUID::regist(m.begin()->first.as<std::string>(), *spec);
                }
                continue;
            }

            if (key == "grid_names") {
                for (ValueMap m : kv.second.as<ValueList>()) {
                    ASSERT(m.size() == 1);
                    const std::unique_ptr<spec::Custom> spec(spec::Custom::make_from_value(m.begin()->second));
                    GridSpecByName::regist(m.begin()->first.as<std::string>(), *spec);
                }
                continue;
            }

            custom->set(key, kv.second);
        }
    }
}


}  // namespace eckit::geo::share
