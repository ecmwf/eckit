// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Figure.h"

#include <algorithm>
#include <map>
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


namespace {


const std::map<std::shared_ptr<Figure>, std::string> KNOWN{
    {std::shared_ptr<Figure>{new figure::Earth}, "earth"},
    {std::shared_ptr<Figure>{new figure::EarthGrib1}, "grib1"},
    {std::shared_ptr<Figure>{new figure::EarthGrs80}, "grs80"},
    {std::shared_ptr<Figure>{new figure::EarthIau1965}, "iau1965"},
    {std::shared_ptr<Figure>{new figure::Sun}, "sun"},
    {std::shared_ptr<Figure>{new figure::EarthWgs84}, "wgs84"},
    {std::shared_ptr<Figure>{new figure::EarthWgs84Sphere}, "wgs84_sphere"},
};


class lock_type {
    inline static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


}  // namespace


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


bool Figure::is_default() const {
    // default figures are known figures (not all)
    auto fn = std::find_if(KNOWN.begin(), KNOWN.end(), [this](const auto& fn) { return *fn.first == *this; });
    return fn != KNOWN.end() && fn->first->is_default();
}


void Figure::fill_spec(spec::Custom& custom) const {
    auto fn = std::find_if(KNOWN.begin(), KNOWN.end(), [this](const auto& fn) { return *fn.first == *this; });
    if (fn != KNOWN.end()) {
        custom.set("figure", fn->second);
        return;
    }

    custom.set("figure", spherical() ? new spec::Custom{{"R", R()}} : new spec::Custom{{"a", a()}, {"b", b()}});
}


Figure* FigureFactory::make_from_string(const std::string& str) {
    std::unique_ptr<Figure::Spec> spec(spec::Custom::make_from_value(YAMLParser::decodeString(str)));
    return build(*spec);
}


const Figure* FigureFactory::make_default() {
    return new figure::Earth;
}


Figure* FigureFactory::build(const Figure::Spec& spec) {
    lock_type lock;

    if (spec.has("figure")) {
        std::string name;
        if (!spec.get("figure", name)) {
            return build(spec.spec("figure"));
        }

        // a figure described inline (e.g. '{"R":6371229}', '{"a":6378137,"b":6356752}'), or by name
        if (auto first = name.find_first_not_of(" \t\n"); first != std::string::npos && name[first] == '{') {
            return make_from_string(name);
        }

        return Factory<Figure>::instance().get(name).create();
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


bool operator==(const Figure& a, const Figure& b) {
    return types::is_approximately_equal(a.a(), b.a()) && types::is_approximately_equal(a.b(), b.b());
}


}  // namespace eckit::geo
