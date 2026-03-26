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


#include "eckit/geo/Area.h"

#include <ostream>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/share/Area.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/spec/Layered.h"


namespace eckit::geo {


namespace {


util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


}  // namespace


const Area::Spec& Area::spec() const {
    if (!spec_) {
        spec_ = std::make_shared<spec::Custom>();
        ASSERT(spec_);

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
    return instance().make_from_spec_(*spec);
}


AreaFactory& AreaFactory::instance() {
    static AreaFactory obj;
    return obj;
}


const Area* AreaFactory::make_from_spec_(const spec::Spec& spec) const {
    lock_type lock;

    std::unique_ptr<Area::Spec> cfg(make_spec_(spec));

    if (std::string type; cfg->get("type", type)) {
        return AreaFactoryType::instance().get(type).create(*cfg);
    }

    list(Log::error() << "Area: cannot build area without 'type', choices are: ");
    throw exception::SpecError("Area: cannot build area without 'type'", Here());
}


Area::Spec* AreaFactory::make_spec_(const Area::Spec& spec) const {
    lock_type lock;
    share::Area::instance();

    auto* cfg = new spec::Layered(spec);
    ASSERT(cfg != nullptr);


    // hardcoded, interpreted options (contributing to areaspec)

    auto back = std::make_unique<spec::Custom>();
    ASSERT(back);

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


void AreaFactory::add_library_(const std::string& lib, Area::Spec* spec) {
    lock_type lock;
    share::Area::instance();

    libraries_.emplace(lib, spec);
}


std::ostream& AreaFactory::list_(std::ostream& out) const {
    lock_type lock;
    share::Area::instance();

    out << AreaSpecByName::instance() << std::endl;
    out << AreaFactoryType::instance() << std::endl;

    out << "Libraries:" << std::endl;
    for (const auto& [name, spec] : libraries_) {
        out << "  " << name << ": " << *spec << std::endl;
    }

    return out;
}


}  // namespace eckit::geo
