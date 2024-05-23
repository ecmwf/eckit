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
    spec_ = std::make_unique<spec::Custom>();

    for (const auto& path : paths) {
        if (path.exists()) {
            load(path);
        }
    }
}


void Grid::load(const PathName& path) {
    auto* custom = dynamic_cast<spec::Custom*>((spec_ ? spec_ : (spec_ = std::make_unique<spec::Custom>())).get());
    ASSERT(custom != nullptr);

    struct SpecByUIDGenerator final : SpecByUID::generator_t {
        explicit SpecByUIDGenerator(spec::Custom* spec) : spec_(spec) { ASSERT(spec_); }
        Spec* spec() const override { return new spec::Custom(spec_->container()); }
        bool match(const spec::Custom& other) const override { return other == *spec_; }

    private:
        std::unique_ptr<spec::Custom> spec_;
    };

    struct SpecByNameGenerator final : SpecByName::generator_t {
        explicit SpecByNameGenerator(spec::Custom* spec) : spec_(spec) { ASSERT(spec_); }
        Spec* spec(SpecByName::generator_t::arg1_t) const override { return new spec::Custom(spec_->container()); }
        bool match(const spec::Custom& other) const override { return other == *spec_; }

    private:
        std::unique_ptr<spec::Custom> spec_;
    };

    if (path.exists()) {
        ValueMap map(YAMLParser::decodeFile(path));

        for (const auto& kv : map) {
            const auto key = kv.first.as<std::string>();

            if (key == "grid_uids") {
                for (ValueMap m : kv.second.as<ValueList>()) {
                    ASSERT(m.size() == 1);
                    SpecByUID::instance().regist(
                        m.begin()->first.as<std::string>(),
                        new SpecByUIDGenerator(spec::Custom::make_from_value(m.begin()->second)));
                }
                continue;
            }

            if (key == "grid_names") {
                for (ValueMap m : kv.second.as<ValueList>()) {
                    ASSERT(m.size() == 1);
                    SpecByName::instance().regist(
                        m.begin()->first.as<std::string>(),
                        new SpecByNameGenerator(spec::Custom::make_from_value(m.begin()->second)));
                }
                continue;
            }

            custom->set(key, kv.second);
        }
    }
}


}  // namespace eckit::geo::etc
