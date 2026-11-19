// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "eckit/geo/Arrangement.h"
#include "eckit/geo/Grid.h"
#include "eckit/geo/Point.h"
#include "eckit/maths/Matrix3.h"


namespace eckit::geo::projection {
class Rotation;
class Stretch;
}  // namespace eckit::geo::projection


namespace eckit::geo::grid {


/**
 * @brief Equiangular gnomonic cubed sphere, of six panels of n x n cells
 * @details Panel vertices (i, j) = (0 ... n, 0 ... n) are at gnomonic (u, v) = (tan a, tan b), a and b equiangular in
 * [-pi/4, pi/4]. The panels (a convention, see Panels) place them on the cube and number the points. Vertices can be
 * stretched ('stretch'), then rotated ('projection' or 'rotation'). Arrangements (one-dimensional shape):
 * - C: cell centres (default), 6 n^2 points, panel by panel, by rows (j) of cells (i);
 * - V: vertices, 6 n^2 + 2 points, numbered by the panels (as shared by panels);
 * - E: edge midpoints, 12 n^2 points, numbered by the panels.
 * Spec: {type: cubed_sphere, n: <n>, arrangement: C|V|E, panels: <name>|{...}}, the arrangement always written.
 */
class CubedSphere : public Grid {
public:

    // -- Class members

    static constexpr size_t PANELS = 6;

    // -- Types

    /// Panel frame, a signed permutation taking a panel point to the cube: (x, y, z) = F (u, v, 1)
    using Frame = maths::Matrix3<double>;

    /// Panel vertices (or cells) [i0, i1) x [j0, j1), by rows (j) of vertices (i)
    struct Block {
        size_t panel;
        size_t i0;
        size_t i1;
        size_t j0;
        size_t j1;

        size_t size() const { return (i1 - i0) * (j1 - j0); }
    };

    /// Panel edges, by block of cells and, for each cell, the edges on its sides: N (vertices (i, j) to (i + 1, j)),
    /// W ((i, j) to (i, j + 1)), S ((i, j + 1) to (i + 1, j + 1)) or E ((i + 1, j) to (i + 1, j + 1))
    struct EdgeBlock {
        Block cells;
        std::string sides;

        size_t size() const { return cells.size() * sides.size(); }
    };

    /**
     * @brief Panels of a cubed sphere (a convention): frames placing the panels on the cube, and the numbering of
     * vertices and edges by blocks, each once
     * @details Written out: {frames: 6 x 9 integers, vertices: 5 integers per block (panel, i0, i1, j0, j1), edges: 5
     * integers per block (of cells), sides: a string per edges block}, checked exactly (frames as signed permutations)
     */
    struct Panels {
        std::array<Frame, PANELS> frames;
        std::vector<Block> vertices;
        std::vector<EdgeBlock> edges;
    };

    /// Panels by name, of n cells per panel side
    using panels_type = std::function<Panels(size_t n)>;

    // -- Constructors

    explicit CubedSphere(const Spec&);

    CubedSphere(size_t n, Arrangement, const std::string& panels, double stretch = 1.,
                std::unique_ptr<const projection::Rotation> = nullptr);

    // -- Destructor

    ~CubedSphere() override;

    // -- Methods

    size_t n() const { return n_; }

    // -- Overridden methods

    [[nodiscard]] Point point(size_t index) const override;

    std::vector<size_t> shape() const override;
    size_t size() const override;
    std::string arrangement() const override;

    [[nodiscard]] BoundingBox* calculate_bbox() const override;

    const std::string& type() const override;

    // -- Class methods

    static void register_panels(const std::string& name, panels_type);

protected:

    // -- Methods

    /// Spec of a named grid (its name and the options the name doesn't encode), false for the complete spec
    virtual bool fill_named_spec(spec::Custom&) const { return false; }

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
    std::unique_ptr<Iterator::State> make_state() const override;

private:

    // -- Types

    struct State;

    // -- Constructors

    CubedSphere(size_t n, Arrangement, std::string name, Panels, double stretch,
                std::unique_ptr<const projection::Rotation>);

    // -- Members

    const size_t n_;
    const Arrangement arrangement_;
    const std::string panels_name_;  // empty if written out
    const Panels panels_;

    /// Gnomonic coordinates of the vertices (see util::equiangular_tangents)
    const std::vector<double> tangents_;

    std::unique_ptr<const projection::Stretch> stretch_;
    std::unique_ptr<const projection::Rotation> rotation_;

    // -- Methods

    PointXYZ vertex(size_t panel, size_t i, size_t j) const;
    PointXYZ cell(size_t panel, size_t i, size_t j) const;
    PointXYZ edge(size_t panel, size_t i, size_t j, char side) const;

    /// Vertex or edge midpoint by its number (see Panels)
    PointXYZ numbered_vertex(size_t) const;
    PointXYZ numbered_edge(size_t) const;
};


}  // namespace eckit::geo::grid
