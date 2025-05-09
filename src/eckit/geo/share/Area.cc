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


#include "eckit/geo/share/Area.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Area.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/value/Value.h"


namespace eckit::geo::share {


const Area& Area::instance() {
    static const Area INSTANCE(LibEcKitGeo::shareArea());
    return INSTANCE;
}


Area::Area(const std::vector<PathName>& paths) : spec_(new spec::Custom) {
    ASSERT(spec_);

    for (const auto& path : paths) {
        if (path.exists()) {
            load(path);
        }
    }
}


void Area::load(const PathName& path) {
    auto* custom = dynamic_cast<spec::Custom*>(spec_.get());
    ASSERT(custom != nullptr);

    struct SpecByNameGenerator final : AreaSpecByName::generator_t {
        explicit SpecByNameGenerator(spec::Custom* spec) : spec_(spec) { ASSERT(spec_); }
        Spec* spec(AreaSpecByName::generator_t::arg1_t) const override { return new spec::Custom(spec_->container()); }
        bool match(const spec::Custom& other) const override { return other == *spec_; }

    private:

        std::unique_ptr<spec::Custom> spec_;
    };

    if (path.exists()) {
        ValueMap map(YAMLParser::decodeFile(path));

        for (const auto& kv : map) {
            const auto key = kv.first.as<std::string>();

            if (key == "area_libraries") {
                for (const auto& [name, spec] : kv.second.as<ValueMap>()) {
                    AreaFactory::add_library(name.as<std::string>(), spec::Custom::make_from_value(spec));
                }
                continue;
            }

            if (key == "area_names") {
                for (ValueMap named : kv.second.as<ValueList>()) {
                    ASSERT(named.size() == 1);
                    AreaSpecByName::instance().regist(
                        named.begin()->first.as<std::string>(),
                        new SpecByNameGenerator(spec::Custom::make_from_value(named.begin()->second)));
                }
                continue;
            }

            custom->set(key, kv.second);
        }
    }
}


}  // namespace eckit::geo::share
