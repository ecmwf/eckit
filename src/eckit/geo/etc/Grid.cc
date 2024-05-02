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

#include "eckit/exception/Exceptions.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/value/Value.h"


namespace eckit::geo::etc {


const Grid& Grid::instance() {
    static const Grid INSTANCE(LibEcKitGeo::etcGrid());
    return INSTANCE;
}


Grid::Grid(const std::vector<PathName>& paths) {
    auto* custom = new spec::Custom;
    spec_.reset(custom);

    for (const auto& path : paths) {
        if (path.exists()) {
            load(path);
        }
    }
}


void Grid::load(const PathName& path) {
    auto* custom = dynamic_cast<spec::Custom*>((spec_ ? spec_ : (spec_ = std::make_unique<spec::Custom>())).get());
    ASSERT(custom != nullptr);

    auto spec_from_value_map = [](const ValueMap& map) {
        auto* custom = new spec::Custom;
        for (const auto& kv : map) {
            custom->set(kv.first, kv.second);
        }
        return custom;
    };

    struct SpecByUIDGenerator final : SpecByUID::generator_t {
        explicit SpecByUIDGenerator(spec::Custom* spec) : spec_(spec) {}
        Spec* spec() const override { return new spec::Custom(*spec_); }
        bool match(const spec::Custom& other) const override { return other == *spec_; }
        std::unique_ptr<spec::Custom> spec_;
    };

    struct SpecByNameGenerator final : SpecByName::generator_t {
        explicit SpecByNameGenerator(spec::Custom* spec) : spec_(spec) {}
        Spec* spec(SpecByName::generator_t::arg1_t) const override { return new spec::Custom(*spec_); }
        bool match(const spec::Custom& other) const override { return other == *spec_; }
        std::unique_ptr<spec::Custom> spec_;
    };

    if (path.exists()) {
        ValueMap map(YAMLParser::decodeFile(path));

        for (const auto& kv : map) {
            const auto key = kv.first.as<std::string>();

            if (key == "grid_uids") {
                for (ValueMap m : static_cast<ValueList>(kv.second)) {
                    ASSERT(m.size() == 1);
                    SpecByUID::instance().regist(m.begin()->first.as<std::string>(),
                                                 new SpecByUIDGenerator(spec_from_value_map(m.begin()->second)));
                }
                continue;
            }

            if (key == "grid_names") {
                for (ValueMap m : static_cast<ValueList>(kv.second)) {
                    ASSERT(m.size() == 1);
                    SpecByName::instance().regist(m.begin()->first.as<std::string>(),
                                                  new SpecByNameGenerator(spec_from_value_map(m.begin()->second)));
                }
                continue;
            }

            custom->set(key, kv.second);
        }
    }
}


}  // namespace eckit::geo::etc
