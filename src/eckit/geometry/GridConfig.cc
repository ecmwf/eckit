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

#include "eckit/config/MappedConfiguration.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geometry/Grid.h"
#include "eckit/geometry/LibEcKitGeometry.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/value/Value.h"


namespace eckit::geometry {


namespace {


template <typename T>
T from_value_t(const Value& from) {
    T to;
    fromValue(to, from);
    return to;
}


void set_config_value(MappedConfiguration& config, const std::string& key, const Value& value) {
    auto all_values = [](const Value& value, const std::function<bool(const Value&)>& pred) -> bool {
        ValueList list(value);
        return std::all_of(list.begin(), list.end(), pred);
    };

    value.isDouble()                                                         ? config.set(key, from_value_t<double>(value))
    : value.isNumber()                                                       ? config.set(key, from_value_t<long>(value))
    : value.isBool()                                                         ? config.set(key, from_value_t<bool>(value))
    : all_values(value, [](const Value& value) { return value.isDouble(); }) ? config.set(key, from_value_t<std::vector<double>>(value))
    : all_values(value, [](const Value& value) { return value.isNumber(); }) ? config.set(key, from_value_t<std::vector<long>>(value))
    : all_values(value, [](const Value& value) { return value.isString(); }) ? config.set(key, from_value_t<std::vector<std::string>>(value))
                                                                             : config.set(key, from_value_t<std::string>(value));
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
    if (!path.exists()) {
        return;
    }

    auto* config = new MappedConfiguration;
    config_.reset(config);

    ValueMap map(YAMLParser::decodeFile(path));

    for (const auto& kv : map) {
        ASSERT(kv.first.isString());
        const auto key = kv.first.as<std::string>();

        if (key == "grid-names") {
            for (ValueMap m : static_cast<ValueList>(kv.second)) {
                ASSERT(m.size() == 1);
                GridFactoryName::insert(m.begin()->first, config_from_value_map(m.begin()->second));
            }
            continue;
        }

        if (key == "grid-uids") {
            for (ValueMap m : static_cast<ValueList>(kv.second)) {
                ASSERT(m.size() == 1);
                GridFactoryUID::insert(m.begin()->first, config_from_value_map(m.begin()->second));
            }
            continue;
        }

        set_config_value(*config, key, kv.second);
    }
}


}  // namespace eckit::geometry
