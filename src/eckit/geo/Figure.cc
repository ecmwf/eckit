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


#include "eckit/geo/Figure.h"

#include <memory>

#include "eckit/exception/Exceptions.h"
#include "eckit/geo/figure/Earth.h"
#include "eckit/geo/figure/OblateSpheroid.h"
#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/geometry/OblateSpheroid.h"
#include "eckit/geo/spec/Custom.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo {


static util::recursive_mutex MUTEX;


class lock_type {
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


double Figure::R() const {
    NOTIMP;
}


double Figure::a() const {
    NOTIMP;
}


double Figure::b() const {
    NOTIMP;
}


spec::Custom* Figure::spec() const {
    auto* custom = new spec::Custom;
    ASSERT(custom != nullptr);

    fill_spec(*custom);
    return custom;
}


std::string Figure::spec_str() const {
    std::unique_ptr<const spec::Custom> custom(spec());
    return custom->str();
}


double Figure::eccentricity() const {
    return geometry::OblateSpheroid::eccentricity(a(), b());
}


double Figure::flattening() const {
    return geometry::OblateSpheroid::flattening(a(), b());
}


void Figure::fill_spec(spec::Custom& custom) const {
    static const std::map<std::shared_ptr<Figure>, std::string> KNOWN{
        {std::shared_ptr<Figure>{new figure::GRS80}, "grs80"},
        {std::shared_ptr<Figure>{new figure::WGS84}, "wgs84"},
    };

    for (const auto& [figure, name] : KNOWN) {
        if (types::is_approximately_equal(figure->a(), a()) && types::is_approximately_equal(figure->b(), b())) {
            custom.set("figure", name);
            return;
        }
    }

    if (types::is_approximately_equal(a(), b())) {
        custom.set("R", R());
    }
    else {
        custom.set("a", a());
        custom.set("b", b());
    }
}


FigureFactory& FigureFactory::instance() {
    static FigureFactory obj;
    return obj;
}


Figure* FigureFactory::make_from_string(const std::string& str) {
    std::unique_ptr<Spec> spec(spec::Custom::make_from_value(YAMLParser::decodeString(str)));
    return instance().make_from_spec_(*spec);
}


Figure* FigureFactory::make_from_spec_(const Spec& spec) const {
    lock_type lock;

    if (std::string figure; spec.get("figure", figure)) {
        return Factory<Figure>::instance().get(figure).create(spec);
    }

    if (double a = 0., b = 0.; spec.get("a", a) && spec.get("b", b)) {
        return types::is_approximately_equal(a, b) ? static_cast<Figure*>(new figure::Sphere(a))
                                                   : new figure::OblateSpheroid(a, b);
    }

    if (double R = 0.; spec.get("R", R)) {
        return new figure::Sphere(R);
    }

    Log::error() << "Figure: cannot build figure without 'R' or 'a', 'b'" << std::endl;
    throw SpecNotFound("Figure: cannot build figure without 'R' or 'a', 'b'", Here());
}


}  // namespace eckit::geo
