// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#include "eckit/geo/grid/CubedSphere.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <map>
#include <numeric>
#include <set>

#include "eckit/geo/Exceptions.h"
#include "eckit/geo/projection/Rotation.h"
#include "eckit/geo/projection/Stretch.h"
#include "eckit/geo/projection/ToFromXYZ.h"
#include "eckit/geo/util.h"
#include "eckit/geo/util/mutex.h"
#include "eckit/spec/Custom.h"
#include "eckit/types/FloatCompare.h"


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
    if (n == 0) {
        throw exception::GridError("CubedSphere: 'n' must be a positive integer", Here());
    }
    return n;
}


Arrangement cubed_sphere_arrangement(Arrangement a) {
    if (a != Arrangement::CUBED_SPHERE_C && a != Arrangement::CUBED_SPHERE_V && a != Arrangement::CUBED_SPHERE_E) {
        throw exception::GridError("CubedSphere: arrangement C, V or E expected", Here());
    }
    return a;
}


/// Panel cell or vertex (i, j)
struct IJ {
    size_t i;
    size_t j;
};


/// Vertices of the edge on a side of a cell (see CubedSphere::EdgeBlock)
std::array<IJ, 2> edge_ends(size_t i, size_t j, char side) {
    switch (side) {
        case 'N':
            return {IJ{i, j}, IJ{i + 1, j}};
        case 'W':
            return {IJ{i, j}, IJ{i, j + 1}};
        case 'S':
            return {IJ{i, j + 1}, IJ{i + 1, j + 1}};
        case 'E':
            return {IJ{i + 1, j}, IJ{i + 1, j + 1}};
        default:
            throw exception::GridError(std::string("CubedSphere: edge side '") + side + "', N, W, S or E expected",
                                       Here());
    }
}


/// Cell of a point of cell centres, panel by panel, by rows (j) of cells (i)
struct CellIndex {
    size_t panel;
    size_t i;
    size_t j;
};


CellIndex cell_index(size_t index, size_t n) {
    const auto cells = n * n;
    return {index / cells, index % cells % n, index % cells / n};
}


template <typename B>
size_t count(const std::vector<B>& blocks) {
    return std::accumulate(blocks.begin(), blocks.end(), size_t{0}, [](size_t s, const B& b) { return s + b.size(); });
}


/// Panel cells (max = n) or vertices (max = n + 1) within the panel
bool within_panel(const CubedSphere::Block& b, size_t max) {
    return b.panel < CubedSphere::PANELS && b.i0 <= b.i1 && b.i1 <= max && b.j0 <= b.j1 && b.j1 <= max;
}


/// Integer cube coordinates of a panel point, F (u, v, w) (exact, so as to identify points shared by panels)
using Lattice = std::array<long long, 3>;

Lattice on_cube(const CubedSphere::Frame& f, long long u, long long v, long long w) {
    std::array<long long, 9> m{};
    std::transform(f.begin(), f.end(), m.begin(), [](double x) { return std::llround(x); });
    return {m[0] * u + m[1] * v + m[2] * w, m[3] * u + m[4] * v + m[5] * w, m[6] * u + m[7] * v + m[8] * w};
}


/// A signed permutation: entries -1, 0 or 1, and columns along distinct axes
bool signed_permutation(const CubedSphere::Frame& f) {
    if (!std::all_of(f.begin(), f.end(), [](double x) { return x == 0. || x == 1. || x == -1.; })) {
        return false;
    }

    std::set<Lattice> axes;
    for (const auto& e : {Lattice{1, 0, 0}, Lattice{0, 1, 0}, Lattice{0, 0, 1}}) {
        const auto c = on_cube(f, e[0], e[1], e[2]);
        axes.insert({std::llabs(c[0]), std::llabs(c[1]), std::llabs(c[2])});
    }

    return axes == std::set<Lattice>{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
}


/// Vertex (i, j) of a panel, on the cube [-n, n]^3
Lattice vertex_on_cube(const CubedSphere::Frame& f, IJ v, size_t n) {
    const auto N = static_cast<long long>(n);
    return on_cube(f, 2 * static_cast<long long>(v.i) - N, 2 * static_cast<long long>(v.j) - N, N);
}


/// Exactly, on the integer lattice: frames centring panels on the six cube faces, and every vertex and edge once
void check_written_out(const CubedSphere::Panels& panels, size_t n) {
    std::set<Lattice> centres;
    for (const auto& f : panels.frames) {
        if (!signed_permutation(f)) {
            throw exception::GridError("CubedSphere: panels 'frames' expected as signed permutations", Here());
        }
        centres.insert(on_cube(f, 0, 0, 1));
    }

    if (centres.size() != CubedSphere::PANELS) {
        throw exception::GridError("CubedSphere: panels 'frames' expected to centre panels on six cube faces", Here());
    }

    if (!panels.vertices.empty()) {
        std::set<Lattice> vertices;
        for (const auto& b : panels.vertices) {
            if (!within_panel(b, n + 1)) {
                throw exception::GridError("CubedSphere: panels 'vertices' expected within the panels", Here());
            }
            for (auto j = b.j0; j < b.j1; ++j) {
                for (auto i = b.i0; i < b.i1; ++i) {
                    vertices.insert(vertex_on_cube(panels.frames[b.panel], {i, j}, n));
                }
            }
        }

        if (vertices.size() != count(panels.vertices) || vertices.size() != CubedSphere::PANELS * n * n + 2) {
            throw exception::GridError("CubedSphere: panels 'vertices' expected to number every vertex once", Here());
        }
    }

    if (!panels.edges.empty()) {
        std::set<Lattice> edges;  // by midpoint (twice)
        for (const auto& e : panels.edges) {
            const auto& b = e.cells;
            if (!within_panel(b, n) || e.sides.empty()) {
                throw exception::GridError("CubedSphere: panels 'edges' expected within the panels", Here());
            }
            for (auto j = b.j0; j < b.j1; ++j) {
                for (auto i = b.i0; i < b.i1; ++i) {
                    for (const auto side : e.sides) {
                        const auto ends = edge_ends(i, j, side);
                        const auto p    = vertex_on_cube(panels.frames[b.panel], ends[0], n);
                        const auto q    = vertex_on_cube(panels.frames[b.panel], ends[1], n);
                        edges.insert({p[0] + q[0], p[1] + q[1], p[2] + q[2]});
                    }
                }
            }
        }

        if (edges.size() != count(panels.edges) || edges.size() != 2 * CubedSphere::PANELS * n * n) {
            throw exception::GridError("CubedSphere: panels 'edges' expected to number every edge once", Here());
        }
    }
}


CubedSphere::Panels panels_by_name(const std::string& name, size_t n) {
    util::lock_guard<util::recursive_mutex> lock(panels_mutex());

    const auto& registry = panels_registry();
    if (auto it = registry.find(name); it != registry.end()) {
        return it->second(n_positive(n));
    }

    throw exception::GridError("CubedSphere: unknown panels '" + name + "'", Here());
}


CubedSphere::Panels panels_written_out(const spec::Spec& spec, size_t n) {
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

    CubedSphere::Panels panels{{frame(0), frame(1), frame(2), frame(3), frame(4), frame(5)}, blocks("vertices"), edges};
    check_written_out(panels, n_positive(n));
    return panels;
}


CubedSphere::Panels panels_from_spec(const spec::Spec& spec) {
    if (std::string name; spec.get("panels", name)) {
        return panels_by_name(name, spec.get_unsigned("n"));
    }

    if (!spec.has("panels")) {
        throw exception::GridError("CubedSphere: 'panels' expected", Here());
    }

    return panels_written_out(spec.spec("panels"), spec.get_unsigned("n"));
}


std::string panels_name_from_spec(const spec::Spec& spec) {
    std::string name;
    return spec.get("panels", name) ? name : "";
}


std::unique_ptr<const projection::Rotation> rotation_from_spec(const spec::Spec& spec) {
    if (!spec.has("projection") && !spec.has("rotation")) {
        return nullptr;
    }

    std::unique_ptr<const Projection> projection(ProjectionFactory::build(spec));
    if (const auto* rotation = dynamic_cast<const projection::Rotation*>(projection.get()); rotation != nullptr) {
        projection.release();
        return std::unique_ptr<const projection::Rotation>(rotation);
    }

    throw exception::GridError("CubedSphere: 'projection' expected as a rotation", Here());
}


PointXYZ normalise(const PointXYZ& p) {
    return p * (1. / std::sqrt(p.X() * p.X() + p.Y() * p.Y() + p.Z() * p.Z()));
}


Point to_lonlat(const PointXYZ& xyz) {
    const auto p = projection::ToFromXYZ::to_lonlat(xyz);
    return PointLonLat::make(p.lon(), p.lat());
}


size_t number_of_points(Arrangement a, size_t n) {
    switch (a) {
        case Arrangement::CUBED_SPHERE_C:
            return CubedSphere::PANELS * n * n;
        case Arrangement::CUBED_SPHERE_V:
            return CubedSphere::PANELS * n * n + 2;
        case Arrangement::CUBED_SPHERE_E:
            return 2 * CubedSphere::PANELS * n * n;
        default:
            NOTIMP;
    }
}


PointXYZ on_panel(const CubedSphere::Frame& f, double u, double v) {
    // (u * u + v * v) is symmetric, so vertices shared by panels are bitwise equal
    return f * PointXYZ{u, v, 1.} * (1. / std::sqrt(1. + (u * u + v * v)));
}


}  // namespace


CubedSphere::CubedSphere(const Spec& spec) :
    CubedSphere(
        n_positive(spec.get_unsigned("n")), arrangement_from_string("CubedSphere", spec.get_string("arrangement", "C")),
        panels_name_from_spec(spec), panels_from_spec(spec), spec.get_double("stretch", 1.), rotation_from_spec(spec)) {
}


CubedSphere::CubedSphere(size_t n, Arrangement a, const std::string& panels, double stretch,
                         std::unique_ptr<const projection::Rotation> r) :
    CubedSphere(n_positive(n), a, panels, panels_by_name(panels, n), stretch, std::move(r)) {}


CubedSphere::CubedSphere(size_t n, Arrangement a, std::string name, Panels panels, double stretch,
                         std::unique_ptr<const projection::Rotation> r) :
    n_(n),
    arrangement_(cubed_sphere_arrangement(a)),
    panels_name_(std::move(name)),
    panels_(std::move(panels)),
    tangents_(util::equiangular_tangents(n_)),
    stretch_(types::is_approximately_equal(stretch, 1.) ? nullptr
                                                        : std::make_unique<const projection::Stretch>(stretch)),
    rotation_(std::move(r)) {
    // the numbering the arrangement needs (named panels are trusted, written-out panels are checked exactly)
    if (arrangement_ == Arrangement::CUBED_SPHERE_V &&
        (count(panels_.vertices) != number_of_points(arrangement_, n_) ||
         !std::all_of(panels_.vertices.begin(), panels_.vertices.end(),
                      [this](const Block& b) { return within_panel(b, n_ + 1); }))) {
        throw exception::GridError("CubedSphere: panels 'vertices' expected to number 6 n^2 + 2 vertices", Here());
    }

    if (arrangement_ == Arrangement::CUBED_SPHERE_E &&
        (count(panels_.edges) != number_of_points(arrangement_, n_) ||
         !std::all_of(panels_.edges.begin(), panels_.edges.end(), [this](const EdgeBlock& e) {
             return within_panel(e.cells, n_) && !e.sides.empty() &&
                    e.sides.find_first_not_of("NWSE") == std::string::npos;
         }))) {
        throw exception::GridError("CubedSphere: panels 'edges' expected to number 12 n^2 edges", Here());
    }

    if (rotation_ && !rotation_->rotated()) {
        rotation_.reset();
    }
}


CubedSphere::~CubedSphere() = default;


std::vector<size_t> CubedSphere::shape() const {
    return {size()};
}


size_t CubedSphere::size() const {
    return number_of_points(arrangement_, n_);
}


std::string CubedSphere::arrangement() const {
    return arrangement_to_string(arrangement_);
}


Grid::BoundingBox* CubedSphere::calculate_bbox() const {
    return new BoundingBox;
}


const std::string& CubedSphere::type() const {
    static const std::string type{"cubed_sphere"};
    return type;
}


void CubedSphere::register_panels(const std::string& name, panels_type panels) {
    util::lock_guard<util::recursive_mutex> lock(panels_mutex());
    panels_registry()[name] = std::move(panels);
}


void CubedSphere::fill_spec(spec::Custom& custom) const {
    Grid::fill_spec(custom);

    if (fill_named_spec(custom)) {
        return;
    }

    custom.set("type", type());
    custom.set("n", n_);

    custom.set("arrangement", arrangement());

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

        std::vector<size_t> vertices;
        for (const auto& b : panels_.vertices) {
            append(vertices, b);
        }

        std::vector<size_t> edges;
        std::vector<std::string> sides;
        for (const auto& e : panels_.edges) {
            append(edges, e.cells);
            sides.push_back(e.sides);
        }

        custom.set("panels",
                   new spec::Custom{{"frames", frames}, {"vertices", vertices}, {"edges", edges}, {"sides", sides}});
    }

    if (stretch_) {
        custom.set("stretch", stretch_->factor());
    }

    if (rotation_) {
        rotation_->fill_grid_spec(custom);
    }
}


PointXYZ CubedSphere::vertex(size_t panel, size_t i, size_t j) const {
    const auto p = on_panel(panels_.frames.at(panel), tangents_.at(i), tangents_.at(j));
    const auto q = stretch_ ? stretch_->fwd(p) : p;
    return rotation_ ? rotation_->inv(q) : q;
}


PointXYZ CubedSphere::cell(size_t panel, size_t i, size_t j) const {
    return normalise(vertex(panel, i, j) + vertex(panel, i + 1, j) + vertex(panel, i + 1, j + 1) +
                     vertex(panel, i, j + 1));
}


PointXYZ CubedSphere::edge(size_t panel, size_t i, size_t j, char side) const {
    const auto ends = edge_ends(i, j, side);
    return normalise(vertex(panel, ends[0].i, ends[0].j) + vertex(panel, ends[1].i, ends[1].j));
}


PointXYZ CubedSphere::numbered_vertex(size_t k) const {
    for (const auto& b : panels_.vertices) {
        if (k < b.size()) {
            const auto ni = b.i1 - b.i0;
            return vertex(b.panel, b.i0 + k % ni, b.j0 + k / ni);
        }
        k -= b.size();
    }
    throw SeriousBug("CubedSphere: index beyond the vertices numbering", Here());
}


PointXYZ CubedSphere::numbered_edge(size_t k) const {
    for (const auto& e : panels_.edges) {
        if (k < e.size()) {
            const auto c  = k / e.sides.size();
            const auto ni = e.cells.i1 - e.cells.i0;
            return edge(e.cells.panel, e.cells.i0 + c % ni, e.cells.j0 + c / ni, e.sides[k % e.sides.size()]);
        }
        k -= e.size();
    }
    throw SeriousBug("CubedSphere: index beyond the edges numbering", Here());
}


Point CubedSphere::point(size_t index) const {
    ASSERT(index < size());

    switch (arrangement_) {
        case Arrangement::CUBED_SPHERE_C: {
            const auto c = cell_index(index, n_);
            return to_lonlat(cell(c.panel, c.i, c.j));
        }
        case Arrangement::CUBED_SPHERE_V:
            return to_lonlat(numbered_vertex(index));
        case Arrangement::CUBED_SPHERE_E:
            return to_lonlat(numbered_edge(index));
        default:
            NOTIMP;
    }
}


/// Cell centres by rows of cells: the vertices of a row of a panel (and of the next), computed once for all its cells
struct CubedSphere::State final : Iterator::State {
    explicit State(const CubedSphere& grid) : grid_(grid) {}

    Point point(size_t index) override {
        ASSERT(index < grid_.size());

        const auto c = cell_index(index, grid_.n_);

        if (lower_.empty() || c.panel != panel_ || c.j != j_) {
            if (!lower_.empty() && c.panel == panel_ && c.j == j_ + 1) {
                lower_.swap(upper_);
            }
            else {
                lower_ = vertices(c.panel, c.j);
            }
            upper_ = vertices(c.panel, c.j + 1);
            panel_ = c.panel;
            j_     = c.j;
        }

        // as cell (the same sum, so the same point)
        return to_lonlat(normalise(lower_[c.i] + lower_[c.i + 1] + upper_[c.i + 1] + upper_[c.i]));
    }

    std::unique_ptr<Iterator::State> clone() const override { return std::make_unique<State>(*this); }

    std::vector<PointXYZ> vertices(size_t panel, size_t j) const {
        std::vector<PointXYZ> row;
        row.reserve(grid_.n_ + 1);
        for (size_t i = 0; i <= grid_.n_; ++i) {
            row.push_back(grid_.vertex(panel, i, j));
        }
        return row;
    }

    const CubedSphere& grid_;
    size_t panel_ = 0;
    size_t j_     = 0;
    std::vector<PointXYZ> lower_;
    std::vector<PointXYZ> upper_;
};


std::unique_ptr<Grid::Iterator::State> CubedSphere::make_state() const {
    // vertices and edges are numbered by blocks (see point)
    if (arrangement_ != Arrangement::CUBED_SPHERE_C) {
        return nullptr;
    }
    return std::make_unique<State>(*this);
}


static const GridRegisterType<CubedSphere> GRIDTYPE("cubed_sphere");


}  // namespace eckit::geo::grid
