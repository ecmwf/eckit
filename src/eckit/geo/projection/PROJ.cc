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


#include "eckit/geo/projection/PROJ.h"

#include <proj.h>

#include <cstdlib>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include "eckit/filesystem/PathName.h"
#include "eckit/geo/Exceptions.h"
#include "eckit/geo/Figure.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo::projection {


static ProjectionRegisterType<PROJ> PROJECTION("proj");


namespace {


constexpr auto CTX              = PJ_DEFAULT_CTX;
constexpr PJ_AREA* DEFAULT_AREA = nullptr;


struct pj_t : std::unique_ptr<PJ, decltype(&proj_destroy)> {
    explicit pj_t(element_type* ptr) : unique_ptr(ptr, &proj_destroy) {
        if (!operator bool()) {
            // common errors are "proj.db not found" or "invalid CRS string"
            const auto err = proj_context_errno(CTX);
            throw exception::ProjectionError(
                "PROJ: failed to create object (err=" + std::to_string(err) + ", description='" +
                    proj_errno_string(err) +
                    "'). Ensure proj.db is available (https://proj.org/en/stable/resource_files.html) or install "
                    "the eckitlib wheel which bundles its own).",
                Here());
        }
    }
};


struct ctx_t : std::unique_ptr<PJ_CONTEXT, decltype(&proj_context_destroy)> {
    explicit ctx_t(element_type* ptr) : unique_ptr(ptr, &proj_context_destroy) {}
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
};


struct LonLat final : Convert {
    PJ_COORD to_coord(const Point& p) const final {
        const auto& q = std::get<PointLonLat>(p);
        return proj_coord(q.lon(), q.lat(), 0, 0);
    }

    Point to_point(const PJ_COORD& c) const final { return PointLonLat::make(c.enu.e, c.enu.n, lon_minimum_); }

    explicit LonLat(double lon_minimum) : lon_minimum_(lon_minimum) {}
    const double lon_minimum_;
};


struct XY final : Convert {
    PJ_COORD to_coord(const Point& p) const final {
        const auto& q = std::get<PointXY>(p);
        return proj_coord(q.X(), q.Y(), 0, 0);
    }

    Point to_point(const PJ_COORD& c) const final { return PointXY{c.xy.x, c.xy.y}; }
};


struct XYZ final : Convert {
    PJ_COORD to_coord(const Point& p) const final {
        const auto& q = std::get<PointXYZ>(p);
        return proj_coord(q.X(), q.Y(), q.Z(), 0);
    }

    Point to_point(const PJ_COORD& c) const final { return PointXYZ{c.xy.x, c.xy.y, c.xyz.z}; }
};


Figure* make_figure(const std::string& proj_str) {
    pj_t identity(proj_create_crs_to_crs(CTX, proj_str.c_str(), proj_str.c_str(), DEFAULT_AREA));

    pj_t crs(proj_get_target_crs(CTX, identity.get()));
    pj_t ellipsoid(proj_get_ellipsoid(CTX, crs.get()));
    ASSERT(ellipsoid);

    double a = 0;
    double b = 0;
    ASSERT(proj_ellipsoid_get_parameters(CTX, ellipsoid.get(), &a, &b, nullptr, nullptr));
    ASSERT(0 < b && b <= a);

    return FigureFactory::build(spec::Custom{{{"a", a}, {"b", b}}});
}


}  // namespace


struct PROJ::Implementation {
    Implementation(PJ* pj_ptr, PJ_CONTEXT* pjc_ptr, Convert* source_ptr, Convert* target_ptr) :
        proj_(pj_ptr), ctx_(pjc_ptr), source_(source_ptr), target_(target_ptr) {
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

private:

    const pj_t proj_;
    const ctx_t ctx_;
    const std::unique_ptr<Convert> source_;
    const std::unique_ptr<Convert> target_;
};


PROJ::PROJ(const std::string& source, const std::string& target, double lon_minimum) :
    Projection(make_figure(target)), source_(source), target_(target) {
    ASSERT(!source_.empty());
    ASSERT(!target_.empty());

    auto make_convert = [lon_minimum](const std::string& string) -> Convert* {
        pj_t identity(proj_create_crs_to_crs(CTX, string.c_str(), string.c_str(), DEFAULT_AREA));
        pj_t crs(proj_get_target_crs(CTX, identity.get()));
        pj_t cs(proj_crs_get_coordinate_system(CTX, crs.get()));
        ASSERT(cs);

        auto type = proj_cs_get_type(CTX, cs.get());
        auto dim  = proj_cs_get_axis_count(CTX, cs.get());

        return type == PJ_CS_TYPE_CARTESIAN && dim == 3   ? static_cast<Convert*>(new XYZ)
               : type == PJ_CS_TYPE_CARTESIAN && dim == 2 ? static_cast<Convert*>(new XY)
               : type == PJ_CS_TYPE_ELLIPSOIDAL           ? static_cast<Convert*>(new LonLat(lon_minimum))
               : type == PJ_CS_TYPE_SPHERICAL             ? static_cast<Convert*>(new LonLat(lon_minimum))
                                                          : NOTIMP;
    };

    // projection, normalised
    pj_t p(proj_create_crs_to_crs(CTX, source_.c_str(), target_.c_str(), DEFAULT_AREA));
    p.reset(proj_normalize_for_visualization(CTX, p.release()));

    implementation_ = std::make_unique<Implementation>(p.release(), CTX, make_convert(source_), make_convert(target_));
    ASSERT(implementation_);
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
        {"figure", "ellps"},
        {"r", "R"},
    };

    static const std::map<std::string, std::string> VALUES{
        {"mercator", "merc"},
        {"reverse_mercator", "merc"},
        {"grs80", "GRS80"},
        {"wgs84", "WGS84"},
    };

    auto rename = [](const std::map<std::string, std::string>& map, const std::string& key) {
        const auto it = map.find(key);
        return it != map.end() ? it->second : key;
    };

    std::set<key_value_type, key_value_compare> set;
    for (const auto& [k, v] : custom.container()) {
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


bool PROJ::proj_database_available(const std::string& fallback_db,
                                   const std::vector<std::string>& fallback_search_paths) {
    // Resolution order:
    //   1. If PROJ can resolve a database on its own (via its compiled-in default search paths, e.g. a system install)
    //   2. If @p fallback_db points at a @c proj.db file, with @p fallback_search_paths used to locate @c proj.ini and
    //   grid files.
    //
    // The fallback is applied via the PROJ per-context API, so it affects only eckit's libproj instance and never leaks
    // into other PROJ users in the process (pyproj, fiona, GDAL, ...). It is a no-op when eckit was built without PROJ
    // support. Intended to be called once, at initialisation, before any PROJ-backed projection is created.

    struct Database {
        Database(const std::string& fallback_db, const std::vector<std::string>& fallback_search_paths) {
            auto database_available = [&]() -> bool {
                const auto previous_level = proj_log_level(CTX, PJ_LOG_NONE);

                auto avail = false;
                try {
                    pj_t crs(proj_create_from_database(CTX, "EPSG", "4326", PJ_CATEGORY_CRS, false, nullptr));
                    avail = static_cast<bool>(crs);
                }
                catch (...) {
                }

                proj_log_level(CTX, previous_level);
                return avail;
            };

            // (1) If PROJ already finds a usable database on its own, leave it alone.
            if (database_available()) {
                available = true;
                return;
            }

            // (2) Only now fall back to the provided (bundled) database, if present.
            if (!fallback_db.empty() && PathName{fallback_db}.exists()) {
                proj_context_set_database_path(CTX, fallback_db.c_str(), nullptr, nullptr);

                std::vector<const char*> paths;
                paths.reserve(fallback_search_paths.size());
                for (const auto& p : fallback_search_paths) {
                    if (!p.empty() && PathName{p}.exists()) {
                        paths.push_back(p.c_str());
                    }
                }

                if (!fallback_search_paths.empty()) {
                    proj_context_set_search_paths(CTX, static_cast<int>(paths.size()), paths.data());
                }

                available = database_available();
            }
        }

        bool available = false;
    } static const DATABASE(fallback_db, fallback_search_paths);

    return DATABASE.available;
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
