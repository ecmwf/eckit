// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <utility>
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
 * @details On a panel, the equiangular coordinates (a, b) in [-pi/4, pi/4] are gnomonic (u, v) = (tan a, tan b), at
 * the nodes (i, j) = (0 ... n, 0 ... n) of the panel cells (i, j) = (0 ... n - 1, 0 ... n - 1). The panels (a
 * convention, see Panels) place and orient the panels on the cube, and number the points. The nodes can be stretched
 * (projection::Stretch, by 'stretching_factor'), then rotated (projection::Rotation, from the rotated sphere, by
 * 'projection' or 'rotation', as other grids). The arrangements are:
 * - C: cell centres, the normalised mean of the cell nodes, shape (6, n, n), panel by panel, by rows (j) of cells (i);
 * - N: nodes, shape (6 n^2 + 2), numbered as the panels;
 * - E: edge midpoints, the normalised mean of the edge nodes, shape (12 n^2), numbered as the panels.
 * The spec is {type: cubed_sphere, n: <n>, arrangement: C|N|E, panels: <name>|{...}}, with the panels by name (as
 * registered) or written out (see Panels).
 */
class CubedSphere : public Grid {
public:

    // -- Class members

    static constexpr size_t PANELS = 6;

    // -- Types

    /// Panel frame, a signed permutation taking a panel point to the cube: (x, y, z) = F (u, v, 1)
    using Frame = maths::Matrix3<double>;

    /// Panel nodes (or cells) [i0, i1) x [j0, j1), by rows (j) of nodes (i)
    struct Block {
        size_t panel;
        size_t i0;
        size_t i1;
        size_t j0;
        size_t j1;

        size_t size() const { return (i1 - i0) * (j1 - j0); }
    };

    /// Panel edges, by block of cells and, for each cell, the edges on its sides: N (nodes (i, j) to (i + 1, j)), W
    /// ((i, j) to (i, j + 1)), S ((i, j + 1) to (i + 1, j + 1)) or E ((i + 1, j) to (i + 1, j + 1))
    struct EdgeBlock {
        Block cells;
        std::string sides;

        size_t size() const { return cells.size() * sides.size(); }
    };

    /**
     * @brief Panels of a cubed sphere (a convention): panel frames, and nodes and edges numbering
     * @details The frames place and orient the panels on the cube (panel i, frames[i]); nodes and edges are numbered
     * by blocks (panel i numbers the nodes and edges it shares with other panels, or not), counting all 6 n^2 + 2
     * nodes and 12 n^2 edges. Written out (spec), the frames are 6 x 9 integers, the nodes blocks 5 integers each
     * (panel, i0, i1, j0, j1), and the edges blocks 5 integers each (of cells) and the sides (strings):
     * {frames: [...], nodes: [...], edges: [...], sides: [...]}.
     */
    struct Panels {
        std::array<Frame, PANELS> frames;
        std::vector<Block> nodes;
        std::vector<EdgeBlock> edges;
    };

    /// Panels by name, of n cells per panel side
    using panels_type = std::function<Panels(size_t n)>;

    // -- Constructors

    explicit CubedSphere(const Spec&);

    /// Takes ownership of the (optional) rotation
    CubedSphere(size_t n, Arrangement, const std::string& panels, double stretching_factor = 1.,
                const projection::Rotation* = nullptr);

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

    static Arrangement arrangement_from_string(const std::string&);

    static void register_panels(const std::string& name, panels_type);

protected:

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;

private:

    // -- Constructors

    CubedSphere(size_t n, Arrangement, std::string name, Panels, double stretching_factor, const projection::Rotation*);

    // -- Members

    const size_t n_;
    const Arrangement arrangement_;
    const std::string panels_name_;  // empty if written out
    const Panels panels_;

    /// Gnomonic coordinates of the nodes (see util::equiangular_tangents)
    const std::vector<double> tangents_;

    std::unique_ptr<const projection::Stretch> stretch_;
    std::unique_ptr<const projection::Rotation> rotation_;

    // -- Methods

    PointXYZ node(size_t panel, size_t i, size_t j) const;
    PointXYZ cell(size_t panel, size_t i, size_t j) const;
    PointXYZ edge(size_t panel, size_t i, size_t j, char side) const;
};


}  // namespace eckit::geo::grid
