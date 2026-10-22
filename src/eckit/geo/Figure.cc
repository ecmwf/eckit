// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Figure.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <memory>
#include <sstream>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/area/BoundingBox.h"
#include "eckit/geo/figure/Earth.h"
#include "eckit/geo/figure/OblateSpheroid.h"
#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/figure/Sun.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


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
    return 2. * M_PI * a() * a() * authalic().qp();
}


double Figure::area(const area::BoundingBox& bbox) const {
    // between parallels, of the authalic latitude (q), Snyder (3-12)
    const auto auth = authalic();
    const auto dlam = bbox.periodic() ? 2. * M_PI : util::DEGREE_TO_RADIAN * (bbox.east() - bbox.west());
    const auto dq   = auth.q(std::sin(util::DEGREE_TO_RADIAN * bbox.north())) -
                    auth.q(std::sin(util::DEGREE_TO_RADIAN * bbox.south()));

    return 0.5 * a() * a() * dlam * dq;
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
    auto to_str = [](double value) {
        std::ostringstream str;
        str.precision(std::numeric_limits<double>::digits10);
        str << value;
        return str.str();
    };

    return *this == figure::EARTH_WGS84   ? "+ellps=WGS84"
           : *this == figure::EARTH_GRS80 ? "+ellps=GRS80"
           : spherical()                  ? "+R=" + to_str(R())
                                          : "+a=" + to_str(a()) + " +b=" + to_str(b());
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


double Figure::Conformal::m(double phi) const {
    // Snyder (14-15), as PROJ pj_msfn
    const auto s = e_ * std::sin(phi);
    return std::cos(phi) / std::sqrt(1. - s * s);
}


double Figure::Conformal::t(double phi) const {
    // Snyder (15-9), as PROJ pj_tsfn
    const auto s = e_ * std::sin(phi);
    return std::tan(M_PI_4 - phi / 2.) / std::pow((1. - s) / (1. + s), e_ / 2.);
}


double Figure::Conformal::phi(double t) const {
    // Latitude of t, Snyder (7-9), as PROJ pj_phi2

    constexpr int max_iter = 15;
    constexpr double eps   = 1e-12;

    auto phi = M_PI_2 - 2. * std::atan(t);
    for (int i = 0; i < max_iter; ++i) {
        const auto s    = e_ * std::sin(phi);
        const auto next = M_PI_2 - 2. * std::atan(t * std::pow((1. - s) / (1. + s), e_ / 2.));
        if (std::abs(next - phi) < eps) {
            return next;
        }
        phi = next;
    }

    throw exception::ProjectionError("Conformal: latitude calculation failed to converge", Here());
}


Figure::Authalic::Authalic(double e) : e_(e), qp_(q(1.)) {}


double Figure::Authalic::q(double sinphi) const {
    // Snyder (3-12), as PROJ pj_qsfn

    if (types::is_approximately_equal(e_, 0.)) {
        return 2. * sinphi;
    }

    const auto es = e_ * sinphi;
    return (1. - e_ * e_) * (sinphi / (1. - es * es) - std::log((1. - es) / (1. + es)) / (2. * e_));
}


double Figure::Authalic::phi(double q) const {
    // Latitude of q, Snyder (3-16), as PROJ pj_authlat

    auto phi = std::asin(std::max(-1., std::min(1., q / 2.)));
    if (types::is_approximately_equal(e_, 0.)) {
        return phi;
    }

    if (std::abs(std::abs(q) - qp_) < 1e-12) {
        return std::copysign(M_PI_2, q);
    }

    constexpr int max_iter = 15;
    constexpr double eps   = 1e-12;

    for (int i = 0; i < max_iter; ++i) {
        const auto s    = std::sin(phi);
        const auto es   = e_ * s;
        const auto dphi = (1. - es * es) * (1. - es * es) / (2. * std::cos(phi)) *
                          (q / (1. - e_ * e_) - s / (1. - es * es) + std::log((1. - es) / (1. + es)) / (2. * e_));
        phi += dphi;
        if (std::abs(dphi) < eps) {
            return phi;
        }
    }

    throw exception::ProjectionError("Authalic: latitude calculation failed to converge", Here());
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


void Figure::fill_projection_spec(spec::Custom& custom) const {
    if (!is_default()) {
        fill_spec(custom);
    }
}


Figure* FigureFactory::make_from_string(const std::string& str) {
    std::unique_ptr<Figure::Spec> spec(spec::Custom::make_from_value(YAMLParser::decodeString(str)));
    return build(*spec);
}


const std::set<std::string>& FigureFactory::keys() {
    static const std::set<std::string> KEYS{
        "figure", "R", "r", "radius", "a", "b", "semi_major_axis", "semi_minor_axis",
    };
    return KEYS;
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

    if (double R = 0.; spec.get("R", R) || spec.get("r", R) || spec.get("radius", R)) {
        return new figure::Sphere(R);
    }

    return const_cast<Figure*>(make_default());
}


bool operator==(const Figure& a, const Figure& b) {
    return types::is_approximately_equal(a.a(), b.a()) && types::is_approximately_equal(a.b(), b.b());
}


}  // namespace eckit::geo
