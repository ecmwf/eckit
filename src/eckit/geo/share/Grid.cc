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


#include "eckit/geo/share/Grid.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/parser/YAMLParser.h"
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
            load(path);
        }
    }
}


void Grid::load(const PathName& path) {
    auto* custom = dynamic_cast<spec::Custom*>(spec_.get());
    ASSERT(custom != nullptr);

    struct SpecByUIDGenerator final : GridSpecByUID::generator_t {
        explicit SpecByUIDGenerator(spec::Custom* spec) : spec_(spec) { ASSERT(spec_); }
        Spec* spec() const override { return new spec::Custom(spec_->container()); }
        bool match(const spec::Custom& other) const override { return other == *spec_; }

    private:

        std::unique_ptr<spec::Custom> spec_;
    };

    struct SpecByNameGenerator final : GridSpecByName::generator_t {
        explicit SpecByNameGenerator(spec::Custom* spec) : spec_(spec) { ASSERT(spec_); }
        Spec* spec(GridSpecByName::generator_t::arg1_t) const override { return new spec::Custom(spec_->container()); }
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
                    GridSpecByUID::instance().regist(
                        m.begin()->first.as<std::string>(),
                        new SpecByUIDGenerator(spec::Custom::make_from_value(m.begin()->second)));
                }
                continue;
            }

            if (key == "grid_names") {
                for (ValueMap m : kv.second.as<ValueList>()) {
                    ASSERT(m.size() == 1);
                    GridSpecByName::instance().regist(
                        m.begin()->first.as<std::string>(),
                        new SpecByNameGenerator(spec::Custom::make_from_value(m.begin()->second)));
                }
                continue;
            }

            custom->set(key, kv.second);
        }
    }
}


}  // namespace eckit::geo::share
