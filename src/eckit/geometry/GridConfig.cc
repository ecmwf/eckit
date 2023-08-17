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


#include "eckit/geometry/GridConfig.h"

#include <algorithm>

#include "eckit/config/MappedConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geometry/Grid.h"
#include "eckit/geometry/LibEcKitGeometry.h"
#include "eckit/geometry/util.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/value/Value.h"


namespace eckit::geometry {


namespace {


template <typename T>
MappedConfiguration::value_type __from_value(const Value& value) {
    T to;
    fromValue(to, value);
    return {to};
}


void set_config_value(MappedConfiguration& config, const std::string& key, const Value& value) {
    using number_type = pl_type::value_type;

    auto list_of = [](const ValueList& list, auto pred) { return std::all_of(list.begin(), list.end(), pred); };

    auto val = value.isList() && list_of(value, [](const Value& v) { return v.isDouble(); })   ? __from_value<std::vector<double>>(value)
               : value.isList() && list_of(value, [](const Value& v) { return v.isNumber(); }) ? __from_value<std::vector<number_type>>(value)
               : value.isList()                                                                ? __from_value<std::vector<std::string>>(value)
               : value.isDouble()                                                              ? __from_value<double>(value)
               : value.isNumber()                                                              ? __from_value<number_type>(value)
                                                                                               : __from_value<std::string>(value);

    std::visit([&](const auto& val) { config.set(key, val); }, val);
}


MappedConfiguration* config_from_value_map(const ValueMap& map) {
    auto* config = new MappedConfiguration;
    for (const auto& kv : map) {
        set_config_value(*config, kv.first, kv.second);
    }
    return config;
}


}  // namespace


const GridConfig& GridConfig::instance() {
    static const GridConfig __instance(LibEcKitGeometry::configFileGrid());
    return __instance;
}


GridConfig::GridConfig(const PathName& path) {
    auto* config = new MappedConfiguration;
    config_.reset(config);

    if (path.exists()) {
        ValueMap map(YAMLParser::decodeFile(path));

        for (const auto& kv : map) {
            const auto key = kv.first.as<std::string>();

            if (key == "grid_names") {
                for (ValueMap m : static_cast<ValueList>(kv.second)) {
                    ASSERT(m.size() == 1);
                    GridFactoryName::insert(m.begin()->first, config_from_value_map(m.begin()->second));
                }
                continue;
            }

            if (key == "grid_uids") {
                for (ValueMap m : static_cast<ValueList>(kv.second)) {
                    ASSERT(m.size() == 1);
                    GridFactoryUID::insert(m.begin()->first, config_from_value_map(m.begin()->second));
                }
                continue;
            }

            set_config_value(*config, key, kv.second);
        }
    }
}


}  // namespace eckit::geometry
