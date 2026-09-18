// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Figure.h"

#include <memory>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/eckit_geo_config.h"
#include "eckit/geo/figure/Earth.h"
#include "eckit/geo/figure/OblateSpheroid.h"
#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/figure/Sun.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"

#if eckit_HAVE_PROJ
#include "eckit/geo/projection/PROJ.h"
#endif


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


double Figure::area() const {
    NOTIMP;
}


double Figure::area(const area::BoundingBox&) const {
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


std::string Figure::proj_str() const {
#if eckit_HAVE_PROJ
    std::unique_ptr<const spec::Custom> custom(spec());
    return projection::PROJ::proj_str(*custom);
#else
    NOTIMP;
#endif
}


bool Figure::spherical() const {
    return types::is_approximately_equal(a(), b());
}


double Figure::eccentricity() const {
    return figure::OblateSpheroid::eccentricity(a(), b());
}


double Figure::flattening() const {
    return figure::OblateSpheroid::flattening(a(), b());
}


void Figure::fill_spec(spec::Custom& custom) const {
    static const std::map<std::shared_ptr<Figure>, std::string> KNOWN{
        {std::shared_ptr<Figure>{new figure::Earth}, "earth"},
        {std::shared_ptr<Figure>{new figure::EarthGrib1}, "grib1"},
        {std::shared_ptr<Figure>{new figure::EarthGrs80}, "grs80"},
        {std::shared_ptr<Figure>{new figure::EarthIau1965}, "iau1965"},
        {std::shared_ptr<Figure>{new figure::Sun}, "sun"},
        {std::shared_ptr<Figure>{new figure::EarthWgs84}, "wgs84"},
        {std::shared_ptr<Figure>{new figure::EarthWgs84Sphere}, "wgs84_sphere"},
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
    std::unique_ptr<Figure::Spec> spec(spec::Custom::make_from_value(YAMLParser::decodeString(str)));
    return instance().make_from_spec_(*spec);
}


const Figure* FigureFactory::make_default() {
    return new figure::Earth;
}


Figure* FigureFactory::make_from_spec_(const Figure::Spec& spec) const {
    lock_type lock;

    if (spec.has("figure")) {
        std::string name;
        return spec.get("figure", name) ? Factory<Figure>::instance().get(name).create()
                                        : make_from_spec_(spec.spec("figure"));
    }

    if (double a = 0., b = 0.;
        (spec.get("a", a) && spec.get("b", b)) || (spec.get("semi_major_axis", a) && spec.get("semi_minor_axis", b))) {
        return types::is_approximately_equal(a, b) ? static_cast<Figure*>(new figure::Sphere(a))
                                                   : new figure::OblateSpheroid(a, b);
    }

    if (double R = 0.; spec.get("R", R) || spec.get("radius", R)) {
        return new figure::Sphere(R);
    }

    return const_cast<Figure*>(make_default());
}


}  // namespace eckit::geo
