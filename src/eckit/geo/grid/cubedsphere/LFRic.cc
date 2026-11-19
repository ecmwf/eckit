// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/cubedsphere/LFRic.h"

#include <cmath>
#include <memory>
#include <regex>
#include <vector>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/projection/Rotation.h"
#include "eckit/geo/projection/Stretch.h"
#include "eckit/geo/util.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


namespace eckit::geo::grid::cubedsphere {


namespace {


const std::string TYPE     = "cubed_sphere_lfric";
constexpr double LFRIC_EPS = 1e-4;  // from rotate_mesh_coords


std::unique_ptr<const projection::Rotation> make_rotation(const PointLonLat& south_pole) {
    const auto lon =
        PointLonLat::normalise_angle_to_minimum(south_pole.lon() + PointLonLat::FLAT_ANGLE, -PointLonLat::FLAT_ANGLE);
    const auto angle = std::abs(util::DEGREE_TO_RADIAN * lon) < LFRIC_EPS ? PointLonLat::FLAT_ANGLE + lon : 0.;
    return std::make_unique<const projection::Rotation>(south_pole, angle);
}


PointLonLat canonical_south_pole(const PointLonLat& south_pole) {
    PointLonLat::assert_latitude_range(south_pole);

    const PointLonLat p{PointLonLat::normalise_angle_to_minimum(south_pole.lon(), 0.), south_pole.lat()};
    return make_rotation(p)->rotated() ? p : SOUTH_POLE;
}


PointLonLat target_south_pole_from_spec(const spec::Spec& spec) {
    auto lonlat = [&spec](const std::string& key) {
        const auto p = PointLonLat::make_from_spec(spec, key);
        PointLonLat::assert_latitude_range(p);
        return p;
    };

    if (spec.has("target_south_pole") + spec.has("target_north_pole") + spec.has("target_null_island") > 1) {
        throw exception::GridError(
            "LFRic: 'target_south_pole', 'target_north_pole' and 'target_null_island' are exclusive", Here());
    }

    if (spec.has("target_south_pole")) {
        return lonlat("target_south_pole");
    }

    if (spec.has("target_north_pole")) {
        // not PointLonLat::antipode, which makes poles longitude 0, but here it is the angle of rotation about them
        auto north_pole = lonlat("target_north_pole");
        return {north_pole.lon() + PointLonLat::FLAT_ANGLE, -north_pole.lat()};
    }

    if (spec.has("target_null_island")) {
        const auto p = lonlat("target_null_island");
        return p.lat() > 0. ? PointLonLat{p.lon(), p.lat() - PointLonLat::RIGHT_ANGLE}
                            : PointLonLat{p.lon() + PointLonLat::FLAT_ANGLE, -PointLonLat::RIGHT_ANGLE - p.lat()};
    }

    return SOUTH_POLE;
}


}  // namespace


LFRic::LFRic(const Spec& spec) :
    LFRic(spec.get_unsigned("n"), arrangement_from_string("CubedSphere", spec.get_string("arrangement", "C")),
          spec.get_double("equatorial_latitude", 0.), target_south_pole_from_spec(spec)) {
    // stretched and rotated by LFRic's own keys, so the generic ones would be a second (and ignored) spelling
    static const std::vector<std::string> keys{"stretch", "projection", "rotation"};
    for (const auto& key : keys) {
        if (spec.has(key)) {
            throw exception::GridError("LFRic: '" + key + "' unsupported (see 'equatorial_latitude' and 'target_*')",
                                       Here());
        }
    }
}


LFRic::LFRic(size_t n, Arrangement a, double equatorial_latitude, const PointLonLat& target_south_pole) :
    CubedSphere(n, a, "lfric", projection::Stretch::factor_from_equatorial_latitude(equatorial_latitude),
                make_rotation(canonical_south_pole(target_south_pole))),
    equatorial_latitude_(equatorial_latitude),
    target_south_pole_(canonical_south_pole(target_south_pole)) {}


std::string LFRic::name() const {
    return "CS-LFR-" + std::to_string(n()) + "-" + arrangement();
}


Grid::Spec* LFRic::spec(const std::string& name) {
    static const std::regex rex("([1-9][0-9]*)(?:[-_]([cve]))?$", std::regex_constants::icase);

    std::smatch match;
    if (!std::regex_search(name, match, rex)) {
        throw exception::GridError("LFRic: unknown name '" + name + "'", Here());
    }

    // the arrangement, cell centres by default
    const auto a = match[2].matched ? match[2].str() : "C";
    return new spec::Custom{{"type", TYPE}, {"n", std::stoul(match[1].str())}, {"arrangement", a}};
}


CubedSphere::Panels LFRic::panels(size_t N) {
    ASSERT(N > 0);

    return {
        // frames: (x, y, z) = F (u, v, 1)
        {{
            {0, 0, 1, 1, 0, 0, 0, -1, 0},
            {-1, 0, 0, 0, 0, 1, 0, -1, 0},
            {0, 0, -1, -1, 0, 0, 0, -1, 0},
            {1, 0, 0, 0, 0, -1, 0, -1, 0},
            {-1, 0, 0, 0, 1, 0, 0, 0, 1},
            {-1, 0, 0, 0, -1, 0, 0, 0, -1},
        }},

        // vertices, as [i0, i1) x [j0, j1) of vertices
        {{0, 0, N, 0, N},
         {1, 0, N, 0, N},
         {2, 0, N, 0, N},
         {3, 0, N, 0, N},
         {4, 1, N, 1, N},
         {5, 0, N, 0, N},
         {5, 0, N, N, N + 1},
         {5, N, N + 1, 0, N + 1}},

        // edges, as [i0, i1) x [j0, j1) of cells, and the sides of each cell
        {{{0, 0, N, 0, 1}, "NWS"},
         {{0, 0, N, 1, N}, "WS"},
         {{1, 0, N, 0, 1}, "NWS"},
         {{1, 0, N, 1, N}, "WS"},
         {{2, 0, N, 0, 1}, "NWS"},
         {{2, 0, N, 1, N}, "WS"},
         {{3, 0, N, 0, 1}, "NWS"},
         {{3, 0, N, 1, N}, "WS"},
         {{4, 0, N, 0, N - 1}, "S"},
         {{4, 0, N - 1, 0, N}, "E"},
         {{5, 0, N, 0, N - 1}, "S"},
         {{5, 0, N - 1, 0, N}, "E"}},
    };
}


bool LFRic::fill_named_spec(spec::Custom& custom) const {
    custom.set("grid", name());

    if (!types::is_approximately_equal(equatorial_latitude_, 0.)) {
        custom.set("equatorial_latitude", equatorial_latitude_);
    }

    if (target_south_pole_.lon() != SOUTH_POLE.lon() || target_south_pole_.lat() != SOUTH_POLE.lat()) {
        custom.set("target_south_pole", std::vector<double>{target_south_pole_.lon(), target_south_pole_.lat()});
    }

    return true;
}


const std::string& LFRic::type() const {
    static const std::string type{TYPE};
    return type;
}


static const struct PanelsRegister {
    PanelsRegister() { CubedSphere::register_panels("lfric", LFRic::panels); }
} PANELS_REGISTER;


static const GridRegisterType<LFRic> GRIDTYPE(TYPE);
static const GridRegisterName<LFRic> GRIDNAME("cs[-_]lfr[-_][1-9][0-9]*([-_][cve])?");


}  // namespace eckit::geo::grid::cubedsphere
