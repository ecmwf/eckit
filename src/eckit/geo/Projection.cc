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

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/LibEcKitGeo.h"
#include "eckit/geo/eckit_geo_config.h"

#if eckit_HAVE_PROJ
#include "eckit/geo/projection/PROJ.h"
#endif


namespace eckit::geo {


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


const Spec& Projection::spec() const {
    if (!spec_) {
        spec_ = std::make_unique<spec::Custom>();
        ASSERT(spec_ != nullptr);

        auto& custom = *spec_;
        fill_spec(custom);
    }

    return *spec_;
}


std::string Projection::spec_str() const {
    return spec().str();
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
