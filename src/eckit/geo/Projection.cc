// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/Projection.h"

#include <array>
#include <limits>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <utility>
#include <variant>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/eckit_geo_config.h"
#include "eckit/geo/projection/None.h"
#include "eckit/geo/projection/Reverse.h"
#include "eckit/geo/projection/Rotation.h"
#include "eckit/geo/share/Projection.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/parser/YAMLParser.h"
#include "eckit/spec/Layered.h"

#if eckit_HAVE_PROJ
#include "eckit/geo/projection/PROJ.h"
#endif


namespace eckit::geo {


namespace {


class lock_type {
    inline static util::recursive_mutex MUTEX;
    util::lock_guard<util::recursive_mutex> lock_guard_{MUTEX};
};


// Projection types interpreting "reverse": true themselves
struct SelfReversing {
    util::recursive_mutex mutex;
    std::set<std::string> types;

    static SelfReversing& instance() {
        static SelfReversing self_reversing;
        return self_reversing;
    }
};


/// Projection types of optional features, if available (eg. "proj" with the PROJ database)
void register_optional_types() {
#if eckit_HAVE_PROJ
    projection::PROJ::register_type();
#endif
}


const Projection* make(const std::string& type, const Projection::Spec& spec) {
    register_optional_types();

    if (spec.has("x_0") || spec.has("y_0")) {
        throw exception::SpecError("ProjectionFactory: no false easting/northing (x_0, y_0), see the grid's 'false'",
                                   Here());
    }

    // e.g. 'proj' without PROJ
    if (!Factory<Projection>::instance().exists(type)) {
        throw exception::ProjectionError("ProjectionFactory: unknown projection type '" + type + "'", Here());
    }

    const auto* projection = Factory<Projection>::instance().get(type).create(spec);
    if (!spec.get_bool("reverse", false)) {
        return projection;
    }

    auto& self_reversing = SelfReversing::instance();
    util::lock_guard<util::recursive_mutex> lock(self_reversing.mutex);

    return self_reversing.types.count(type) == 0 ? new projection::Reverse(projection) : projection;
}


/// Tag carrying a point type, see visit_point_type
template <typename P>
struct point_tag {
    using type = P;
};


/// Call f(point_tag<P>{}) for the point type P, the Point alternative at the given index (not the empty point)
template <typename F, size_t I = 1>
void visit_point_type(size_t index, F&& f) {
    if constexpr (I < std::variant_size_v<Point>) {
        if (index == I) {
            f(point_tag<std::variant_alternative_t<I, Point>>{});
            return;
        }
        visit_point_type<F, I + 1>(index, std::forward<F>(f));
    }
    else {
        throw BadValue("Projection: invalid point type (" + std::to_string(index) + ")", Here());
    }
}


const std::vector<std::string>& coordinates(size_t index) {
    const std::vector<std::string>* names = nullptr;
    visit_point_type(index, [&names](auto tag) { names = &point_coordinates<typename decltype(tag)::type>(); });
    return *names;
}


size_t dims(size_t index) {
    size_t n = 0;
    visit_point_type(index, [&n](auto tag) { n = decltype(tag)::type::DIMS; });
    return n;
}


/// Check the vectors of coordinates of points of a point type (v3 is empty for points of 2 coordinates)
void check_vectors(const std::vector<double>& v1, const std::vector<double>& v2, const std::vector<double>& v3,
                   size_t type) {
    const auto n = v1.size();
    if (dims(type) == 3 ? v2.size() == n && v3.size() == n : v2.size() == n && v3.empty()) {
        return;
    }

    std::string names;
    for (const auto& name : coordinates(type)) {
        names += (names.empty() ? "" : ", ") + name;
    }

    throw BadValue("Projection: expected coordinates (" + names + ") of the same size, got sizes " +
                       std::to_string(v1.size()) + ", " + std::to_string(v2.size()) + ", " + std::to_string(v3.size()),
                   Here());
}


/// Project points one by one, from/to point types known only at run time (resolved once); failures are NaN
template <typename F>
std::vector<std::vector<double>> project_points(const std::vector<double>& v1, const std::vector<double>& v2,
                                                const std::vector<double>& v3, size_t from, size_t to, F project) {
    const std::array<const std::vector<double>*, 3> in{&v1, &v2, &v3};
    const auto n = v1.size();

    std::vector<std::vector<double>> out(dims(to), std::vector<double>(n));

    visit_point_type(from, [&](auto from_tag) {
        visit_point_type(to, [&](auto to_tag) {
            using P = typename decltype(from_tag)::type;
            using Q = typename decltype(to_tag)::type;

            for (size_t i = 0; i < n; ++i) {
                P p;
                for (size_t k = 0; k < P::DIMS; ++k) {
                    p.data()[k] = (*in[k])[i];
                }

                try {
                    const auto q  = project(Point{p});
                    const auto& r = std::get<Q>(q);  // throws if the projection doesn't honour its point types
                    for (size_t k = 0; k < Q::DIMS; ++k) {
                        out[k][i] = r.data()[k];
                    }
                }
                catch (const eckit::Exception&) {
                    for (auto& o : out) {
                        o[i] = std::numeric_limits<double>::quiet_NaN();
                    }
                }
            }
        });
    });

    return out;
}


}  // namespace


Projection::Projection(Figure* ptr, const Point& source, const Point& target) :
    figure_(ptr != nullptr ? ptr : FigureFactory::make_default()) {
    ASSERT(figure_);
    point_types(source, target);
}


Projection::Projection(const Projection& other, bool reverse) :
    figure_(other.figure_),
    source_(reverse ? other.target_ : other.source_),
    target_(reverse ? other.source_ : other.target_) {
    ASSERT(figure_);
}


std::vector<std::vector<double>> Projection::fwd(const std::vector<double>& v1, const std::vector<double>& v2,
                                                 const std::vector<double>& v3) const {
    check_vectors(v1, v2, v3, source_);
    return fwd_vector(v1, v2, v3);
}


std::vector<std::vector<double>> Projection::inv(const std::vector<double>& v1, const std::vector<double>& v2,
                                                 const std::vector<double>& v3) const {
    check_vectors(v1, v2, v3, target_);
    return inv_vector(v1, v2, v3);
}


const std::vector<std::string>& Projection::source_point_coordinates() const {
    return coordinates(source_);
}


const std::vector<std::string>& Projection::target_point_coordinates() const {
    return coordinates(target_);
}


Point Projection::from_grid_xy(double x, double y) const {
    static const size_t LONLAT = Point{PointLonLat{}}.index();
    return target_ == LONLAT ? inv(PointLonLat{x, y}) : inv(PointXY{x, y});
}


void Projection::point_types(const Point& source, const Point& target) {
    // validates the point types (the empty point is not a point type)
    dims(source.index());
    dims(target.index());

    source_ = source.index();
    target_ = target.index();
}


void Projection::point_types_from(const Projection& first, const Projection& last) {
    source_ = first.source_;
    target_ = last.target_;
}


std::vector<std::vector<double>> Projection::fwd_vector(const std::vector<double>& v1, const std::vector<double>& v2,
                                                        const std::vector<double>& v3) const {
    return project_points(v1, v2, v3, source_, target_, [this](const Point& p) { return fwd(p); });
}


std::vector<std::vector<double>> Projection::inv_vector(const std::vector<double>& v1, const std::vector<double>& v2,
                                                        const std::vector<double>& v3) const {
    return project_points(v1, v2, v3, target_, source_, [this](const Point& q) { return inv(q); });
}


const Projection::Spec& Projection::spec() const {
    if (!spec_) {
        spec_ = std::make_shared<spec::Custom>();

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
    // not spec(), which can be a projection name, and the figure (even if a default, omitted from spec)
    spec::Custom custom;
    fill_spec(custom);
    figure().fill_spec(custom);
    return proj_str(custom);
}


std::string Projection::proj_str(const spec::Custom& custom) {
    const auto type = custom.get_string("type", "");

    // the coordinate reference system other than (lon, lat)
    if (type == "proj") {
        const auto source = custom.get_string("source", proj_default());
        return source != proj_default() ? source : custom.get_string("target", proj_default());
    }

    // projection types with a PROJ equivalent
    static const std::map<std::string, std::string> NAMES{
        {"", "longlat"},  // the default projection
        {"none", "longlat"},
        {"aea", "aea"},
        {"albers", "aea"},
        {"eqc", "eqc"},
        {"plate-carree", "eqc"},
        {"laea", "laea"},
        {"lambert_azimuthal_equal_area", "laea"},
        {"lcc", "lcc"},
        {"lambert", "lcc"},
        {"lambert_lam", "lcc"},
        {"ll-to-xyz", "cart"},
        {"merc", "merc"},
        {"mercator", "merc"},
        {"rotation", "ob_tran"},
        {"stere", "stere"},
        {"polar_stereographic", "stere"},
    };

    const auto name = NAMES.find(type);
    if (name == NAMES.end()) {
        throw exception::ProjectionError("PROJ: no equivalent to projection type '" + type + "'", Here());
    }

    // key "proj" comes first in string, then the others (sorted)
    auto compare = [](const std::string& a, const std::string& b) {
        return a != b && (a == "proj" || b == "proj") ? a == "proj" : a < b;
    };

    std::map<std::string, std::string, decltype(compare)> keys(compare);
    auto str = [](double value) { return to_string(spec::Custom::value_type{value}); };

    // figure comes last in string (default figure if not specified)
    std::unique_ptr<const Figure> figure(FigureFactory::build(custom));
    ASSERT(figure);
    auto figure_str = figure->proj_str();

    keys["proj"] = name->second;

    if (name->second == "ob_tran") {
        const auto south_pole = custom.get_double_vector("south_pole", {0., -90.});
        const auto angle      = custom.get_double("rotation_angle", 0.);

        keys["o_proj"]  = "longlat";
        keys["lon_0"]   = str(south_pole[0]);
        keys["o_lat_p"] = str(-south_pole[1]);
        if (angle != 0.) {
            keys["o_lon_p"] = str(angle);
        }
    }
    else {
        for (const auto& [k, v] : custom.container()) {
            if (k != "type" && k != "reverse" && FigureFactory::keys().count(k) == 0) {
                keys[k] = to_string(v);
            }
        }

        if (name->second == "lcc" && keys.count("lat_0") == 0) {
            keys["lat_0"] = "0";  // (PROJ defaults to lat_1, without lat_2)
        }

        if (name->second == "eqc") {
            // (x, y) in degrees, independent of the figure: on the sphere of its semi-major axis (PROJ eqc is on the
            // ellipsoid, and a sphere of radius 180/pi is not of Earth)
            keys["to_meter"] = str(figure->a() * util::DEGREE_TO_RADIAN);
            figure_str       = "+R=" + str(figure->a());
        }
    }

    std::string proj;
    for (const auto& [key, value] : keys) {
        proj += "+" + key + "=" + value + " ";
    }

    return proj + figure_str;
}


const std::string& Projection::proj_default() {
    static const std::string DEFAULT = "EPSG:4326";  // WGS84, latitude/longitude coordinate system
    return DEFAULT;
}


bool Projection::is_default() const {
    return *this == projection_default();
}


const Projection& Projection::projection_default() {
    static const projection::None PROJECTION;
    return PROJECTION;
}


const Projection* ProjectionFactory::make_default() {
    return ProjectionFactory::build(Projection::projection_default().spec());
}


void Projection::fill_spec(spec::Custom& custom) const {
    figure_->fill_projection_spec(custom);
}


void Projection::fill_grid_spec(spec::Custom& custom) const {
    if (is_default()) {
        return;
    }

    spec::Custom projection_spec;
    fill_spec(projection_spec);

    if (projection_spec.only(Figure::className())) {
        custom.set(projection_spec);
        return;
    }

    if (std::string name; ProjectionSpecByName::instance().match(projection_spec, name)) {
        custom.set(className(), name);
        return;
    }

    custom.set(className(), projection_spec);
}


const Projection* ProjectionFactory::make_from_string(const std::string& str) {
    std::unique_ptr<Projection::Spec> spec(spec::Custom::make_from_value(YAMLParser::decodeString(str)));
    return build(*spec);
}


const Projection* ProjectionFactory::build(const Projection::Spec& spec) {
    lock_type lock;

    // an explicit 'projection' has to name its type, or be a name (see projection.yaml)
    if (spec.has("projection")) {
        if (std::string name; spec.get("projection", name)) {
            share::Projection::instance();
            if (!ProjectionSpecByName::instance().matches(name)) {
                throw exception::ProjectionError("ProjectionFactory: unknown projection name '" + name + "'", Here());
            }

            std::unique_ptr<Projection::Spec> cfg(ProjectionSpecByName::instance().match(name).spec(name));
            return make(cfg->get_string("type"), *cfg);
        }

        const auto& cfg = spec.spec("projection");
        return make(cfg.get_string("type"), cfg);
    }

    if (spec.has("rotation")) {
        if (auto* rotation = projection::Rotation::make_from_spec(spec); rotation != nullptr) {
            return rotation;
        }
    }

    std::unique_ptr<Projection::Spec> cfg(make_spec(spec));

    // NOTE: a 'type' that isn't a projection's is somebody else's (eg. a grid's)
    if (std::string type; cfg->get("type", type) && has_type(type)) {
        return make(type, *cfg);
    }

    return make("eqc", *cfg);  // the default
}


void ProjectionFactory::register_self_reversing(const std::string& type) {
    auto& self_reversing = SelfReversing::instance();
    util::lock_guard<util::recursive_mutex> lock(self_reversing.mutex);

    ASSERT(self_reversing.types.insert(type).second);
}


Projection::Spec* ProjectionFactory::make_spec(const Projection::Spec& spec) {
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


bool ProjectionFactory::has_type(const std::string& type) {
    register_optional_types();
    return Factory<Projection>::instance().exists(type);
}


std::ostream& ProjectionFactory::list(std::ostream& out) {
    register_optional_types();
    lock_type lock;
    share::Projection::instance();

    out << ProjectionSpecByName::instance() << std::endl;
    out << Factory<Projection>::instance() << std::endl;

    return out;
}


}  // namespace eckit::geo
