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


#include "eckit/geo/GridConfig.h"

#include <algorithm>
#include <memory>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/spec/MappedConfiguration.h"
#include "eckit/geo/util.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/value/Value.h"


namespace eckit::geo {


namespace {


template <typename T>
spec::MappedConfiguration::value_type __from_value(const Value& value) {
    T to;
    fromValue(to, value);
    return {to};
}


void set_config_value(spec::MappedConfiguration& config, const std::string& key, const Value& value) {
    using number_type = pl_type::value_type;

    auto list_of = [](const ValueList& list, auto pred) { return std::all_of(list.begin(), list.end(), pred); };

    auto val = value.isList() && list_of(value, [](const Value& v) { return v.isDouble(); })
                   ? __from_value<std::vector<double>>(value)
               : value.isList() && list_of(value, [](const Value& v) { return v.isNumber(); })
                   ? __from_value<std::vector<number_type>>(value)
               : value.isList()   ? __from_value<std::vector<std::string>>(value)
               : value.isDouble() ? __from_value<double>(value)
               : value.isNumber() ? __from_value<number_type>(value)
                                  : __from_value<std::string>(value);

    std::visit([&](const auto& val) { config.set(key, val); }, val);
}


spec::MappedConfiguration* config_from_value_map(const ValueMap& map) {
    auto* config = new spec::MappedConfiguration;
    for (const auto& kv : map) {
        set_config_value(*config, kv.first, kv.second);
    }
    return config;
}


}  // namespace


const GridConfig& GridConfig::instance() {
    static const GridConfig __instance(LibEcKitGeo::configFileGrid());
    return __instance;
}


GridConfig::GridConfig(const PathName& path) {
    auto* config = new spec::MappedConfiguration;
    config_.reset(config);

    struct ConfigurationFromUID final : GridConfigurationUID::configurator_t {
        explicit ConfigurationFromUID(spec::MappedConfiguration* config) :
            config_(config) {}
        Configuration* config() const override { return new spec::MappedConfiguration(*config_); }
        std::unique_ptr<spec::MappedConfiguration> config_;
    };

    struct ConfigurationFromName final : GridConfigurationName::configurator_t {
        explicit ConfigurationFromName(spec::MappedConfiguration* config) :
            config_(config) {}
        Configuration* config(GridConfigurationName::configurator_t::arg1_t) const override {
            return new spec::MappedConfiguration(*config_);
        }
        std::unique_ptr<spec::MappedConfiguration> config_;
    };

    if (path.exists()) {
        ValueMap map(YAMLParser::decodeFile(path));

        for (const auto& kv : map) {
            const auto key = kv.first.as<std::string>();

            if (key == "grid_uids") {
                for (ValueMap m : static_cast<ValueList>(kv.second)) {
                    ASSERT(m.size() == 1);
                    GridConfigurationUID::instance().regist(
                        m.begin()->first.as<std::string>(),
                        new ConfigurationFromUID(config_from_value_map(m.begin()->second)));
                }
                continue;
            }

            if (key == "grid_names") {
                for (ValueMap m : static_cast<ValueList>(kv.second)) {
                    ASSERT(m.size() == 1);
                    GridConfigurationName::instance().regist(
                        m.begin()->first.as<std::string>(),
                        new ConfigurationFromName(config_from_value_map(m.begin()->second)));
                }
                continue;
            }

            set_config_value(*config, key, kv.second);
        }
    }
}


}  // namespace eckit::geo
