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


#include "eckit/geo/etc/Grid.h"

#include <algorithm>
#include <memory>

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/value/Value.h"


namespace eckit::geo::etc {


namespace {


template <typename T>
spec::Custom::value_type __from_value(const Value& value) {
    T to;
    fromValue(to, value);
    return {to};
}


void set_config_value(spec::Custom& config, const std::string& key, const Value& value) {
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


spec::Custom* config_from_value_map(const ValueMap& map) {
    auto* config = new spec::Custom;
    for (const auto& kv : map) {
        set_config_value(*config, kv.first, kv.second);
    }
    return config;
}


}  // namespace


const Grid& Grid::instance() {
    static const Grid __instance(LibEcKitGeo::configFileGrid());
    return __instance;
}


Grid::Grid(const PathName& path) {
    auto* config = new spec::Custom;
    config_.reset(config);

    struct SpecUID final : SpecByUID::configurator_t {
        explicit SpecUID(spec::Custom* config) :
            config_(config) {}
        Spec* config() const override { return new spec::Custom(*config_); }
        std::unique_ptr<spec::Custom> config_;
    };

    struct SpecName final : SpecByName::configurator_t {
        explicit SpecName(spec::Custom* config) :
            config_(config) {}
        Spec* config(SpecByName::configurator_t::arg1_t) const override { return new spec::Custom(*config_); }
        std::unique_ptr<spec::Custom> config_;
    };

    if (path.exists()) {
        ValueMap map(YAMLParser::decodeFile(path));

        for (const auto& kv : map) {
            const auto key = kv.first.as<std::string>();

            if (key == "grid_uids") {
                for (ValueMap m : static_cast<ValueList>(kv.second)) {
                    ASSERT(m.size() == 1);
                    SpecByUID::instance().regist(m.begin()->first.as<std::string>(),
                                                 new SpecUID(config_from_value_map(m.begin()->second)));
                }
                continue;
            }

            if (key == "grid_names") {
                for (ValueMap m : static_cast<ValueList>(kv.second)) {
                    ASSERT(m.size() == 1);
                    SpecByName::instance().regist(m.begin()->first.as<std::string>(),
                                                  new SpecName(config_from_value_map(m.begin()->second)));
                }
                continue;
            }

            set_config_value(*config, key, kv.second);
        }
    }
}


}  // namespace eckit::geo::etc
