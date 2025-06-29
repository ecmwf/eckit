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


#include "eckit/geo/Projection.h"

#include <ostream>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/eckit_geo_config.h"
#include "eckit/geo/figure/Earth.h"
#include "eckit/geo/share/Projection.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/spec/Layered.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/types/FloatCompare.h"

#if eckit_HAVE_PROJ
#include "eckit/geo/projection/PROJ.h"
#endif


namespace eckit::geo {


namespace {


util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


}  // namespace


Projection::Projection(Figure* figure_ptr) : figure_(figure_ptr != nullptr ? figure_ptr : new figure::Earth) {
    ASSERT(figure_);
}


const Figure& Projection::figure() const {
    ASSERT(figure_);
    return *figure_;
}


const Spec& Projection::spec() const {
    if (!spec_) {
        spec_ = std::make_shared<spec::Custom>();
        ASSERT(spec_);

        auto& custom = *spec_;
        fill_spec(custom);

        if (std::string name; !custom.empty() && ProjectionSpecByName::instance().match(custom, name)) {
            custom.clear();
            custom.set(className(), name);
        }
    }

    return *spec_;
}


std::string Projection::proj_str() const {
#if eckit_HAVE_PROJ
    return projection::PROJ::proj_str(dynamic_cast<const spec::Custom&>(spec()));
#else
    NOTIMP;
#endif
}


Projection* Projection::make_from_spec(const Spec& spec) {
    return ProjectionFactoryType::instance()
        .get(spec.get_string(LibEcKitGeo::proj() ? "proj" : "projection"))
        .create(spec);
}


void Projection::fill_spec(spec::Custom& custom) const {
    figure_->fill_spec(custom);

    if (!types::is_approximately_equal(false_.X, 0.)) {
        custom.set("x_0", false_.X);
    }

    if (!types::is_approximately_equal(false_.Y, 0.)) {
        custom.set("y_0", false_.Y);
    }
}


const Projection* ProjectionFactory::make_from_string(const std::string& str) {
    std::unique_ptr<Spec> spec(spec::Custom::make_from_value(YAMLParser::decodeString(str)));
    return instance().make_from_spec_(*spec);
}


ProjectionFactory& ProjectionFactory::instance() {
    static ProjectionFactory obj;
    return obj;
}


const Projection* ProjectionFactory::make_from_spec_(const Spec& spec) const {
    lock_type lock;

    std::unique_ptr<Spec> cfg(make_spec_(spec));

    if (std::string type; cfg->get("type", type)) {
        return ProjectionFactoryType::instance().get(type).create(*cfg);
    }

    list(Log::error() << "Projection: cannot build projection without 'type', choices are: ");
    throw exception::SpecError("Projection: cannot build projection without 'type'", Here());
}


Spec* ProjectionFactory::make_spec_(const Spec& spec) const {
    lock_type lock;
    share::Projection::instance();

    auto* cfg = new spec::Layered(spec);
    ASSERT(cfg != nullptr);


    // hardcoded, interpreted options (contributing to projectionspec)

    if (spec.has("proj")) {
        cfg->push_back(new spec::Custom{{"type", "proj"}});
    }
    else if (spec.has("rotation")) {
        std::vector<double> rotation;
        spec.get("rotation", rotation);
        cfg->push_back(new spec::Custom{{"type", "rotation"}, {"rotation", rotation}});
    }

    return cfg;
}


std::ostream& ProjectionFactory::list_(std::ostream& out) const {
    lock_type lock;
    share::Projection::instance();

    out << ProjectionSpecByName::instance() << std::endl;
    out << ProjectionFactoryType::instance() << std::endl;

    return out;
}


}  // namespace eckit::geo
