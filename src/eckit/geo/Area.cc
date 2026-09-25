// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Area.h"

#include <map>
#include <memory>
#include <ostream>
#include <string>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/share/Area.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/log/Log.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/spec/Layered.h"


namespace eckit::geo {


namespace {


class lock_type {
    inline static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


std::map<std::string, std::unique_ptr<Area::Spec>>& libraries() {
    static std::map<std::string, std::unique_ptr<Area::Spec>> LIBRARIES;
    return LIBRARIES;
}


}  // namespace


const Area::Spec& Area::spec() const {
    if (!spec_) {
        spec_ = std::make_shared<spec::Custom>();

        auto& custom = *spec_;
        fill_spec(custom);

        if (std::string name; !custom.empty() && AreaSpecByName::instance().match(custom, name)) {
            custom.clear();
            custom.set(className(), name);
        }
    }

    return *spec_;
}


bool Area::intersects(area::BoundingBox&) const {
    NOTIMP;
}


bool Area::contains(const Point&) const {
    NOTIMP;
}


double Area::area() const {
    NOTIMP;
}


const Area& Area::area_default() {
    return area::BoundingBox::bounding_box_default();
}


const Area* AreaFactory::make_from_string(const std::string& str) {
    std::unique_ptr<Area::Spec> spec(spec::Custom::make_from_value(YAMLParser::decodeString(str)));
    return build(*spec);
}


const Area* AreaFactory::build(const spec::Spec& spec) {
    lock_type lock;

    std::unique_ptr<Area::Spec> cfg(make_spec(spec));

    if (std::string type; cfg->get("type", type)) {
        return Factory<Area>::instance().get(type).create(*cfg);
    }

    list(Log::error() << "Area: cannot build area without 'type', choices are: ");
    throw exception::SpecError("Area: cannot build area without 'type'", Here());
}


Area::Spec* AreaFactory::make_spec(const Area::Spec& spec) {
    lock_type lock;
    share::Area::instance();

    auto* cfg = new spec::Layered(spec);
    ASSERT(cfg != nullptr);


    // hardcoded, interpreted options (contributing to areaspec)

    auto back = std::make_unique<spec::Custom>();

    cfg->push_back(new spec::Custom{{"type", "bounding_box"}});

    // if (cfg->has("north") || cfg->has("east") || cfg->has("south") || cfg->has("west")) {
    //     // back->set("type", "reduced_gg");
    // }

    // if (std::vector<double> area; cfg->get("area", area) && area.size() == 4) {
    //     back->set("type", "regular_ll");
    // }

    if (!back->empty()) {
        cfg->push_back(back.release());
    }

    return cfg;
}


void AreaFactory::add_library(const std::string& lib, Area::Spec* spec) {
    lock_type lock;
    share::Area::instance();

    libraries().emplace(lib, spec);
}


std::ostream& AreaFactory::list(std::ostream& out) {
    lock_type lock;
    share::Area::instance();

    out << AreaSpecByName::instance() << std::endl;
    out << Factory<Area>::instance() << std::endl;

    out << "Libraries:" << std::endl;
    for (const auto& [name, spec] : libraries()) {
        out << "  " << name << ": " << *spec << std::endl;
    }

    return out;
}


}  // namespace eckit::geo
