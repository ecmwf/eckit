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
#include "eckit/geo/share/Area.h"
#include "eckit/geo/spec/Layered.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/parser/YAMLParser.h"


namespace eckit::geo {


namespace {


util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


}  // namespace


const Spec& Area::spec() const {
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


bool Area::contains(const PointLonLat&) const {
    NOTIMP;
}


double Area::area() const {
    NOTIMP;
}


const Area* AreaFactory::make_from_string(const std::string& str) {
    std::unique_ptr<Spec> spec(spec::Custom::make_from_value(YAMLParser::decodeString(str)));
    return instance().make_from_spec_(*spec);
}


AreaFactory& AreaFactory::instance() {
    static AreaFactory obj;
    return obj;
}


const Area* AreaFactory::make_from_spec_(const Spec& spec) const {
    lock_type lock;

    std::unique_ptr<Spec> cfg(make_spec_(spec));

    if (std::string type; cfg->get("type", type)) {
        return AreaFactoryType::instance().get(type).create(*cfg);
    }

    list(Log::error() << "Area: cannot build area without 'type', choices are: ");
    throw exception::SpecError("Area: cannot build area without 'type'", Here());
}


Spec* AreaFactory::make_spec_(const Spec& spec) const {
    lock_type lock;
    share::Area::instance();

    auto* cfg = new spec::Layered(spec);
    ASSERT(cfg != nullptr);


    // hardcoded, interpreted options (contributing to areaspec)

    cfg->push_back(new spec::Custom{{"type", "bounding_box"}});

    return cfg;
}


void AreaFactory::add_library_(const std::string& lib, Spec* spec) {
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
