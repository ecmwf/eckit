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

#include <memory>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/eckit_geo_config.h"
#include "eckit/geo/spec/Custom.h"

#if eckit_HAVE_PROJ
#include "eckit/geo/projection/PROJ.h"
#endif


namespace eckit::geo {


ProjectionProblem::ProjectionProblem(const std::string& what, const CodeLocation& loc) : Exception(loc) {
    reason("ProjectionProblem: [" + what + "], in " + loc.asString());
};


Figure* Projection::make_figure() const {
    NOTIMP;
}


const Figure& Projection::figure() const {
    if (!figure_) {
        figure_.reset(make_figure());
        ASSERT(figure_);
    }

    return *figure_;
}


spec::Custom* Projection::spec() const {
    auto* custom = new spec::Custom;
    ASSERT(custom != nullptr);

    fill_spec(*custom);
    return custom;
}


std::string Projection::spec_str() const {
    std::unique_ptr<const spec::Custom> custom(spec());
    return custom->str();
}


std::string Projection::proj_str() const {
#if eckit_HAVE_PROJ
    std::unique_ptr<spec::Custom> custom(spec());
    return projection::PROJ::proj_str(*custom);
#else
    NOTIMP;
#endif
}


Projection* Projection::make_from_spec(const Spec& spec) {
    return ProjectionFactory::instance().get(spec.get_string(LibEcKitGeo::proj() ? "proj" : "projection")).create(spec);
}


void Projection::fill_spec(spec::Custom&) const {
    NOTIMP;
}


}  // namespace eckit::geo
