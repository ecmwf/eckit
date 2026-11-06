// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/CubedSphere.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/figure/Sphere.h"
#include "eckit/geo/projection/Rotation.h"
#include "eckit/geo/projection/Stretch.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/spec/Custom.h"


namespace eckit::geo::grid {


namespace {


util::recursive_mutex& panels_mutex() {
    static util::recursive_mutex mutex;
    return mutex;
}


std::map<std::string, CubedSphere::panels_type>& panels_registry() {
    static std::map<std::string, CubedSphere::panels_type> panels;
    return panels;
}


size_t n_positive(size_t n) {
    return n > 0 ? n : throw exception::GridError("CubedSphere: 'n' must be a positive integer", Here());
}


CubedSphere::Panels panels_by_name(const std::string& name, size_t n) {
    util::lock_guard<util::recursive_mutex> lock(panels_mutex());

    const auto& registry = panels_registry();
    if (auto it = registry.find(name); it != registry.end()) {
        return it->second(n_positive(n));
    }

    throw exception::GridError("CubedSphere: unknown panels '" + name + "'", Here());
}


CubedSphere::Panels panels_written_out(const spec::Spec& spec) {
    auto blocks = [&spec](const std::string& key) {
        const auto v = spec.get_unsigned_vector(key, {});
        if (v.size() % 5 != 0) {
            throw exception::GridError("CubedSphere: panels '" + key + "' expected as 5 integers per block", Here());
        }

        std::vector<CubedSphere::Block> b;
        for (size_t k = 0; k < v.size(); k += 5) {
            b.push_back({v[k], v[k + 1], v[k + 2], v[k + 3], v[k + 4]});
        }
        return b;
    };

    const auto f = spec.get_long_vector("frames");
    if (f.size() != CubedSphere::PANELS * 9) {
        throw exception::GridError("CubedSphere: panels 'frames' expected as 6 x 9 integers", Here());
    }

    auto frame = [&f](size_t p) -> CubedSphere::Frame {
        auto x = [&f, p](size_t k) { return static_cast<double>(f[9 * p + k]); };
        return {x(0), x(1), x(2), x(3), x(4), x(5), x(6), x(7), x(8)};
    };

    const auto cells = blocks("edges");

    std::vector<std::string> sides;
    spec.get("sides", sides);
    if (sides.size() != cells.size()) {
        throw exception::GridError("CubedSphere: panels 'edges' and 'sides' expected of the same size", Here());
    }

    std::vector<CubedSphere::EdgeBlock> edges;
    for (size_t k = 0; k < cells.size(); ++k) {
        edges.push_back({cells[k], sides[k]});
    }

    return {{frame(0), frame(1), frame(2), frame(3), frame(4), frame(5)}, blocks("nodes"), edges};
}


CubedSphere::Panels panels_from_spec(const spec::Spec& spec) {
    if (std::string name; spec.get("panels", name)) {
        return panels_by_name(name, spec.get_unsigned("n"));
    }

    if (!spec.has("panels")) {
        throw exception::GridError("CubedSphere: 'panels' expected", Here());
    }

    return panels_written_out(spec.spec("panels"));
}


std::string panels_name_from_spec(const spec::Spec& spec) {
    std::string name;
    return spec.get("panels", name) ? name : "";
}


const projection::Rotation* rotation_from_spec(const spec::Spec& spec) {
    if (!spec.has("projection") && !spec.has("rotation")) {
        return nullptr;
    }

    std::unique_ptr<const Projection> projection(ProjectionFactory::build(spec));
    if (const auto* rotation = dynamic_cast<const projection::Rotation*>(projection.get()); rotation != nullptr) {
        projection.release();
        return rotation;
    }

    throw exception::GridError("CubedSphere: 'projection' expected as a rotation", Here());
}


PointXYZ normalise(const PointXYZ& p) {
    return p * (1. / std::sqrt(p.X() * p.X() + p.Y() * p.Y() + p.Z() * p.Z()));
}


PointXYZ on_panel(const CubedSphere::Frame& f, double u, double v) {
    // (u * u + v * v) is symmetric, so nodes shared by panels are bitwise equal
    return f * PointXYZ{u, v, 1.} * (1. / std::sqrt(1. + (u * u + v * v)));
}


template <typename B>
size_t count(const std::vector<B>& blocks) {
    return std::accumulate(blocks.begin(), blocks.end(), size_t{0}, [](size_t s, const B& b) { return s + b.size(); });
}


}  // namespace


CubedSphere::CubedSphere(const Spec& spec) :
    CubedSphere(n_positive(spec.get_unsigned("n")), arrangement_from_string(spec.get_string("arrangement", "C")),
                panels_name_from_spec(spec), panels_from_spec(spec), spec.get_double("stretching_factor", 1.),
                rotation_from_spec(spec)) {}


CubedSphere::CubedSphere(size_t n, Arrangement a, const std::string& panels, double stretching_factor,
                         const projection::Rotation* r) :
    CubedSphere(n_positive(n), a, panels, panels_by_name(panels, n), stretching_factor, r) {}


CubedSphere::CubedSphere(size_t n, Arrangement a, std::string name, Panels panels, double stretching_factor,
                         const projection::Rotation* r) :
    n_(n),
    arrangement_(a),
    panels_name_(std::move(name)),
    panels_(std::move(panels)),
    tangents_(util::equiangular_tangents(n_)),
    stretch_(stretching_factor == 1. ? nullptr : new projection::Stretch(stretching_factor)),
    rotation_(r) {
    auto valid = [](const Block& b, size_t max) {
        return b.panel < PANELS && b.i0 <= b.i1 && b.i1 <= max && b.j0 <= b.j1 && b.j1 <= max;
    };

    if (arrangement_ == Arrangement::CUBED_SPHERE_N &&
        !(count(panels_.nodes) == size() &&
          std::all_of(panels_.nodes.begin(), panels_.nodes.end(), [&](const Block& b) { return valid(b, n_ + 1); }))) {
        throw exception::GridError("CubedSphere: panels 'nodes' expected to number 6 n^2 + 2 nodes", Here());
    }

    if (arrangement_ == Arrangement::CUBED_SPHERE_E &&
        !(count(panels_.edges) == size() &&
          std::all_of(panels_.edges.begin(), panels_.edges.end(), [&](const EdgeBlock& b) {
              return valid(b.cells, n_) && !b.sides.empty() && b.sides.find_first_not_of("NWSE") == std::string::npos;
          }))) {
        throw exception::GridError("CubedSphere: panels 'edges' expected to number 12 n^2 edges", Here());
    }

    if (rotation_ && !rotation_->rotated()) {
        rotation_.reset();
    }
}


CubedSphere::~CubedSphere() = default;


std::vector<size_t> CubedSphere::shape() const {
    if (arrangement_ == Arrangement::CUBED_SPHERE_C) {
        return {PANELS, n_, n_};
    }
    return {size()};
}


size_t CubedSphere::size() const {
    return arrangement_ == Arrangement::CUBED_SPHERE_C   ? PANELS * n_ * n_
           : arrangement_ == Arrangement::CUBED_SPHERE_N ? PANELS * n_ * n_ + 2
                                                         : 2 * PANELS * n_ * n_;
}


std::string CubedSphere::arrangement() const {
    return arrangement_ == Arrangement::CUBED_SPHERE_C ? "C" : arrangement_ == Arrangement::CUBED_SPHERE_N ? "N" : "E";
}


Grid::BoundingBox* CubedSphere::calculate_bbox() const {
    return new BoundingBox;
}


const std::string& CubedSphere::type() const {
    static const std::string type{"cubed_sphere"};
    return type;
}


Arrangement CubedSphere::arrangement_from_string(const std::string& str) {
    return str == "C"   ? Arrangement::CUBED_SPHERE_C
           : str == "N" ? Arrangement::CUBED_SPHERE_N
           : str == "E" ? Arrangement::CUBED_SPHERE_E
                        : throw exception::GridError("CubedSphere: unsupported arrangement '" + str + "'", Here());
}


void CubedSphere::register_panels(const std::string& name, panels_type panels) {
    util::lock_guard<util::recursive_mutex> lock(panels_mutex());
    panels_registry()[name] = std::move(panels);
}


void CubedSphere::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);

    custom.set("type", type());
    custom.set("n", n_);

    if (arrangement_ != Arrangement::CUBED_SPHERE_C) {
        custom.set("arrangement", arrangement());
    }

    if (!panels_name_.empty()) {
        custom.set("panels", panels_name_);
    }
    else {
        std::vector<long> frames;
        for (const auto& f : panels_.frames) {
            for (const auto& x : f) {
                frames.push_back(static_cast<long>(x));
            }
        }

        auto append = [](std::vector<size_t>& v, const Block& b) {
            v.insert(v.end(), {b.panel, b.i0, b.i1, b.j0, b.j1});
        };

        std::vector<size_t> nodes;
        for (const auto& b : panels_.nodes) {
            append(nodes, b);
        }

        std::vector<size_t> edges;
        std::vector<std::string> sides;
        for (const auto& [b, s] : panels_.edges) {
            append(edges, b);
            sides.push_back(s);
        }

        custom.set("panels",
                   new spec::Custom{{"frames", frames}, {"nodes", nodes}, {"edges", edges}, {"sides", sides}});
    }

    if (stretch_) {
        custom.set("stretching_factor", stretch_->factor());
    }

    if (rotation_) {
        rotation_->fill_grid_spec(custom);
    }
}


PointXYZ CubedSphere::node(size_t panel, size_t i, size_t j) const {
    const auto p = on_panel(panels_.frames.at(panel), tangents_.at(i), tangents_.at(j));
    const auto q = stretch_ ? stretch_->fwd(p) : p;
    return rotation_ ? rotation_->inv(q) : q;
}


PointXYZ CubedSphere::cell(size_t panel, size_t i, size_t j) const {
    return normalise(node(panel, i, j) + node(panel, i + 1, j) + node(panel, i + 1, j + 1) + node(panel, i, j + 1));
}


PointXYZ CubedSphere::edge(size_t panel, size_t i, size_t j, char side) const {
    return side == 'N'   ? normalise(node(panel, i, j) + node(panel, i + 1, j))
           : side == 'W' ? normalise(node(panel, i, j) + node(panel, i, j + 1))
           : side == 'S' ? normalise(node(panel, i, j + 1) + node(panel, i + 1, j + 1))
                         : normalise(node(panel, i + 1, j) + node(panel, i + 1, j + 1));
}


Point CubedSphere::point(size_t index) const {
    ASSERT(index < size());

    const auto xyz = [this](size_t k) {
        if (arrangement_ == Arrangement::CUBED_SPHERE_C) {
            const auto cells = n_ * n_;
            return cell(k / cells, k % cells % n_, k % cells / n_);
        }

        if (arrangement_ == Arrangement::CUBED_SPHERE_N) {
            for (const auto& b : panels_.nodes) {
                if (k < b.size()) {
                    const auto ni = b.i1 - b.i0;
                    return node(b.panel, b.i0 + k % ni, b.j0 + k / ni);
                }
                k -= b.size();
            }
        }

        for (const auto& [b, sides] : panels_.edges) {
            if (k < b.size() * sides.size()) {
                const auto c  = k / sides.size();
                const auto ni = b.i1 - b.i0;
                return edge(b.panel, b.i0 + c % ni, b.j0 + c / ni, sides[k % sides.size()]);
            }
            k -= b.size() * sides.size();
        }

        NOTIMP;
    }(index);

    const auto p = figure::Sphere::convertCartesianToSpherical(1., xyz);
    return PointLonLat::make(p.lon(), p.lat());
}


static const GridRegisterType<CubedSphere> GRIDTYPE("cubed_sphere");


}  // namespace eckit::geo::grid
