// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/projection/PROJ.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

#include <proj.h>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/geo/Point.h"
#include "eckit/geo/figure/Earth.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::projection {


static ProjectionRegisterType<PROJ> PROJECTION("proj");


namespace {


PJ_AREA* AREA       = nullptr;
PJ_CONTEXT* CONTEXT = nullptr;


PJ_CONTEXT* ctx() {
    return CONTEXT == nullptr ? (CONTEXT = proj_context_create()) : CONTEXT;
}


PJ_AREA* area() {
    return AREA;  // not specific
}


void proj_reset() {
    if (CONTEXT != nullptr) {
        proj_context_destroy(CONTEXT);
        CONTEXT = nullptr;
    }
    ctx();
    area();
}


struct pj_t : std::unique_ptr<PJ, decltype(&proj_destroy)> {
    explicit pj_t(element_type* ptr) : unique_ptr(ptr, &proj_destroy) {
        if (!operator bool()) {
            // common errors are "proj.db not found" or "invalid CRS string"
            const auto err = proj_context_errno(ctx());
            throw exception::ProjectionError("PROJ: failed to create object (err=" + std::to_string(err) +
                                                 ", description='" + proj_errno_string(err) + "')",
                                             Here());
        }
    }
};


struct Convert {
    Convert()          = default;
    virtual ~Convert() = default;

    Convert(const Convert&)        = delete;
    Convert(Convert&&)             = delete;
    void operator=(const Convert&) = delete;
    void operator=(Convert&&)      = delete;

    virtual PJ_COORD to_coord(const Point&) const = 0;
    virtual Point to_point(const PJ_COORD&) const = 0;
    virtual Point point() const                   = 0;  // an example point, of the point type
    virtual size_t dims() const                   = 0;

    /// Normalise projected coordinates (one vector per coordinate), as to_point does
    virtual void normalise(std::vector<std::vector<double>>&) const {}
};


struct LonLat final : Convert {
    PJ_COORD to_coord(const Point& p) const final {
        const auto& q = std::get<PointLonLat>(p);
        return proj_coord(q.lon(), q.lat(), 0, 0);
    }

    Point to_point(const PJ_COORD& c) const final { return PointLonLat::make(c.enu.e, c.enu.n, lon_minimum_); }

    Point point() const final { return PointLonLat{}; }
    size_t dims() const final { return PointLonLat::DIMS; }

    void normalise(std::vector<std::vector<double>>& c) const final {
        auto& lon = c[0];
        auto& lat = c[1];
        for (size_t i = 0; i < lon.size(); ++i) {
            if (std::isfinite(lon[i]) && std::isfinite(lat[i])) {
                const auto q = PointLonLat::make(lon[i], lat[i], lon_minimum_);
                lon[i]       = q.lon();
                lat[i]       = q.lat();
            }
        }
    }

    explicit LonLat(double lon_minimum) : lon_minimum_(lon_minimum) {}
    const double lon_minimum_;
};


struct XY final : Convert {
    PJ_COORD to_coord(const Point& p) const final {
        const auto& q = std::get<PointXY>(p);
        return proj_coord(q.X(), q.Y(), 0, 0);
    }

    Point to_point(const PJ_COORD& c) const final { return PointXY{c.xy.x, c.xy.y}; }

    Point point() const final { return PointXY{}; }
    size_t dims() const final { return PointXY::DIMS; }
};


struct XYZ final : Convert {
    PJ_COORD to_coord(const Point& p) const final {
        const auto& q = std::get<PointXYZ>(p);
        return proj_coord(q.X(), q.Y(), q.Z(), 0);
    }

    Point to_point(const PJ_COORD& c) const final { return PointXYZ{c.xy.x, c.xy.y, c.xyz.z}; }

    Point point() const final { return PointXYZ{}; }
    size_t dims() const final { return PointXYZ::DIMS; }
};


Figure* make_figure(const std::string& proj_str) {
    pj_t identity(proj_create_crs_to_crs(ctx(), proj_str.c_str(), proj_str.c_str(), area()));

    pj_t crs(proj_get_target_crs(ctx(), identity.get()));
    pj_t ellipsoid(proj_get_ellipsoid(ctx(), crs.get()));
    ASSERT(ellipsoid);

    double a = 0;
    double b = 0;
    ASSERT(proj_ellipsoid_get_parameters(ctx(), ellipsoid.get(), &a, &b, nullptr, nullptr));
    ASSERT(0 < b && b <= a);

    return FigureFactory::build(spec::Custom{{{"a", a}, {"b", b}}});
}


}  // namespace


struct PROJ::Implementation {
    Implementation(PJ* pj_ptr, Convert* source_ptr, Convert* target_ptr) :
        proj_(pj_ptr), source_(source_ptr), target_(target_ptr) {
        ASSERT(proj_);
        ASSERT(source_);
        ASSERT(target_);
    }

    inline Point fwd(const Point& p) const {
        return target_->to_point(proj_trans(proj_.get(), PJ_FWD, source_->to_coord(p)));
    }

    inline Point inv(const Point& p) const {
        return source_->to_point(proj_trans(proj_.get(), PJ_INV, target_->to_coord(p)));
    }

    /// Project points (one vector per coordinate), in the given direction; points failing to project are NaN
    std::vector<std::vector<double>> trans(PJ_DIRECTION direction, const std::vector<double>& v1,
                                           const std::vector<double>& v2, const std::vector<double>& v3) const {
        const auto& from = direction == PJ_FWD ? *source_ : *target_;
        const auto& to   = direction == PJ_FWD ? *target_ : *source_;

        // PROJ transforms coordinates x, y (and z) in place
        const auto n = v1.size();
        const auto m = std::max(from.dims(), to.dims());
        ASSERT(2 <= m && m <= 3);

        std::vector<std::vector<double>> c{v1, v2};
        if (m == 3) {
            c.emplace_back(from.dims() == 3 ? v3 : std::vector<double>(n, 0.));
        }

        constexpr auto stride = sizeof(double);
        proj_trans_generic(proj_.get(), direction, c[0].data(), stride, n, c[1].data(), stride, n,
                           m == 3 ? c[2].data() : nullptr, stride, m == 3 ? n : 0, nullptr, 0, 0);

        c.resize(to.dims());

        // points that fail to project (HUGE_VAL) result in NaN
        for (size_t i = 0; i < n; ++i) {
            if (std::any_of(c.begin(), c.end(), [i](const auto& ci) { return !std::isfinite(ci[i]); })) {
                for (auto& ci : c) {
                    ci[i] = std::numeric_limits<double>::quiet_NaN();
                }
            }
        }

        to.normalise(c);
        return c;
    }

    inline Point source_point() const { return source_->point(); }
    inline Point target_point() const { return target_->point(); }

private:

    const pj_t proj_;
    const std::unique_ptr<const Convert> source_;
    const std::unique_ptr<const Convert> target_;
};


PROJ::PROJ(const std::string& source, const std::string& target, double lon_minimum) :
    Projection(make_figure(target)), source_(source), target_(target) {
    ASSERT(!source_.empty());
    ASSERT(!target_.empty());

    auto make_convert = [lon_minimum](const std::string& string) -> Convert* {
        pj_t identity(proj_create_crs_to_crs(ctx(), string.c_str(), string.c_str(), area()));
        pj_t crs(proj_get_target_crs(ctx(), identity.get()));
        pj_t cs(proj_crs_get_coordinate_system(ctx(), crs.get()));
        ASSERT(cs);

        auto type = proj_cs_get_type(ctx(), cs.get());
        auto dim  = proj_cs_get_axis_count(ctx(), cs.get());

        return type == PJ_CS_TYPE_CARTESIAN && dim == 3   ? static_cast<Convert*>(new XYZ)
               : type == PJ_CS_TYPE_CARTESIAN && dim == 2 ? static_cast<Convert*>(new XY)
               : type == PJ_CS_TYPE_ELLIPSOIDAL           ? static_cast<Convert*>(new LonLat(lon_minimum))
               : type == PJ_CS_TYPE_SPHERICAL             ? static_cast<Convert*>(new LonLat(lon_minimum))
                                                          : NOTIMP;
    };

    // projection, normalised
    pj_t p(proj_create_crs_to_crs(ctx(), source_.c_str(), target_.c_str(), area()));
    p.reset(proj_normalize_for_visualization(ctx(), p.release()));

    implementation_ = std::make_unique<Implementation>(p.release(), make_convert(source_), make_convert(target_));
    point_types(implementation_->source_point(), implementation_->target_point());
}


PROJ::PROJ(const Spec& spec) :
    PROJ(spec.get_string("source", proj_default()), spec.get_string("target", spec.get_string("proj", proj_default())),
         spec.get_double("lon_minimum", 0)) {}


PROJ::~PROJ() = default;


const std::string& PROJ::type() const {
    static const std::string type{"proj"};
    return type;
}


Point PROJ::fwd(const Point& p) const {
    return implementation_->fwd(p);
}


Point PROJ::inv(const Point& q) const {
    return implementation_->inv(q);
}


std::vector<std::vector<double>> PROJ::fwd_vector(const std::vector<double>& v1, const std::vector<double>& v2,
                                                  const std::vector<double>& v3) const {
    return implementation_->trans(PJ_FWD, v1, v2, v3);
}


std::vector<std::vector<double>> PROJ::inv_vector(const std::vector<double>& v1, const std::vector<double>& v2,
                                                  const std::vector<double>& v3) const {
    return implementation_->trans(PJ_INV, v1, v2, v3);
}


std::string PROJ::proj_str(const spec::Custom& custom) {
    using key_value_type = std::pair<std::string, std::string>;
    using keys_type      = std::vector<std::string>;

    struct key_value_compare {
        bool operator()(const key_value_type& a, const key_value_type& b) const {
            if (a.first != b.first) {
                // keys that come first in string
                for (const auto& key : keys_type{"proj"}) {
                    if (a.first == key || b.first == key) {
                        return a.first == key;
                    }
                }

                // keys that come last in string
                for (const auto& key : keys_type{"R", "a", "b"}) {
                    if (a.first == key || b.first == key) {
                        return b.first == key;
                    }
                }
            }

            return a < b;
        };
    };

    static const std::map<std::string, std::string> KEYS{
        {"type", "proj"},
    };

    static const std::map<std::string, std::string> VALUES{
        {"mercator", "merc"},
        {"reverse_mercator", "merc"},
    };

    auto rename = [](const std::map<std::string, std::string>& map, const std::string& key) {
        const auto it = map.find(key);
        return it != map.end() ? it->second : key;
    };

    auto to_str = [](double value) {
        std::ostringstream str;
        str.precision(15);
        str << value;
        return str.str();
    };

    static const keys_type FIGURE_KEYS{"figure", "R", "r", "radius", "a", "b", "semi_major_axis", "semi_minor_axis"};

    struct ProjFigure : std::unique_ptr<Figure> {
        ProjFigure(const spec::Spec& custom) :
            unique_ptr(std::any_of(FIGURE_KEYS.begin(), FIGURE_KEYS.end(),
                                   [&custom](const auto& key) { return custom.has(key); })
                           ? FigureFactory::build(custom)
                           : static_cast<Figure*>(new figure::Earth)) {
            ASSERT(operator bool());
        }

        bool is_approximately_equal(const Figure& other) const {
            return types::is_approximately_equal(get()->a(), other.a()) &&
                   types::is_approximately_equal(get()->b(), other.b());
        };
    } fig(custom);


    std::set<key_value_type, key_value_compare> set;

    if (fig.is_approximately_equal(figure::EARTH_WGS84)) {
        set.emplace("ellps", "WGS84");
    }
    else if (fig.is_approximately_equal(figure::EARTH_GRS80)) {
        set.emplace("ellps", "GRS80");
    }
    else if (fig->spherical()) {
        set.emplace("R", to_str(fig->R()));
    }
    else {
        set.emplace("a", to_str(fig->a()));
        set.emplace("b", to_str(fig->b()));
    }

    for (const auto& [k, v] : custom.container()) {
        if (std::find(FIGURE_KEYS.begin(), FIGURE_KEYS.end(), k) != FIGURE_KEYS.end()) {
            continue;
        }

        if (const auto& key = rename(KEYS, k); !key.empty()) {
            const auto& value = rename(VALUES, to_string(v));
            set.emplace(key, value);
        }
    }

    std::string str;
    const auto* sep = "+";
    for (const auto& [key, value] : set) {
        str += sep + key + "=" + value;
        sep = " +";
    }

    return str;
}


const std::string& PROJ::proj_default() {
    static const std::string DEFAULT = "EPSG:4326";  // WGS84, latitude/longitude coordinate system
    return DEFAULT;
}


bool PROJ::projdb_is_available() {
    struct MuteLog {
        MuteLog() : previous_(proj_log_level(ctx(), PJ_LOG_NONE)) {}
        ~MuteLog() { proj_log_level(ctx(), previous_); }
        const PJ_LOG_LEVEL previous_;
    } mute_log;

    // Note: not using pj_t, which throws on failure (failure is a possible outcome)
    std::unique_ptr<pj_t::element_type, pj_t::deleter_type> crs(
        proj_create_from_database(ctx(), "EPSG", "4326", PJ_CATEGORY_CRS, 0, nullptr), &proj_destroy);

    return static_cast<bool>(crs);
}


void PROJ::projdb_set_search_paths(const std::string& db_path, const std::vector<std::string>& search_paths) {
    // Recreate context so the new paths takes effect (an already-open database is reset)
    proj_reset();

    if (!db_path.empty()) {
        proj_context_set_database_path(ctx(), db_path.c_str(), nullptr, nullptr);
    }

    if (!search_paths.empty()) {
        std::vector<const char*> paths;
        paths.reserve(search_paths.size());
        for (const auto& p : search_paths) {
            paths.push_back(p.c_str());
        }

        proj_context_set_search_paths(ctx(), static_cast<int>(search_paths.size()), paths.data());
    }
}


void PROJ::projdb_reset() {
    // a fresh context re-resolves the database from the environment / compiled-in defaults.
    proj_reset();
}


void PROJ::fill_spec(spec::Custom& custom) const {
    custom.set("type", "proj");
    if (source_ != proj_default()) {
        custom.set("source", source_);
    }
    if (target_ != proj_default()) {
        custom.set("target", target_);
    }
}


}  // namespace eckit::geo::projection
