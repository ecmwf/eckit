// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <string>

#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/grid/CubedSphere.h"


namespace eckit::geo::grid::cubedsphere {


/**
 * @brief LFRic (Met Office) cubed sphere, as LFRic's cubedsphere_mesh_generator (gencube_ps_mod)
 * @details A cubed sphere of panels 'lfric' (see panels): panels 1 to 4 band the equator eastwards, panel 1 centred
 * on (0, 0), and panels 5 and 6 are centred on the North and South Poles; cells, nodes and edges are numbered as the
 * generator numbers them. The nodes can be stretched (Schmidt transform, by 'equatorial_latitude'), then rotated (as
 * rotate_mesh_coords, by 'target_south_pole', 'target_north_pole' or 'target_null_island', [lon, lat], recorded as
 * 'target_south_pole'). Names are CS-LFR-<n> (cell centres), CS-LFR-<n>-N (nodes) and CS-LFR-<n>-E (edge midpoints),
 * case-insensitive, by hyphen or underscore.
 */
class LFRic final : public CubedSphere {
public:

    // -- Constructors

    explicit LFRic(const Spec&);
    explicit LFRic(size_t n, Arrangement = Arrangement::CUBED_SPHERE_C, double equatorial_latitude = 0.,
                   const PointLonLat& target_south_pole = SOUTH_POLE);

    // -- Methods

    double equatorial_latitude() const { return equatorial_latitude_; }
    const PointLonLat& target_south_pole() const { return target_south_pole_; }

    // -- Overridden methods

    std::string name() const override;
    const std::string& type() const override;

    // -- Class methods

    [[nodiscard]] static Spec* spec(const std::string& name);

    /**
     * @brief Panels 'lfric', of n cells per panel side (as gencube_ps_mod)
     * @details The frames (as calc_coords) orient the panel coordinates (u, v) along the cells numbering: eastwards
     * and southwards on panels 1 to 4. Cells are numbered panel by panel from the NW corner, by rows (southwards) of
     * cells (as calc_adjacency); nodes (as calc_face_to_vert) are the NW node of each cell of panels 1 to 4, then the
     * inner nodes of panel 5, then all nodes of panel 6; edges (as calc_edges) are, on panels 1 to 4, the N, W and S
     * edges of the cells of the top row then the W and S edges of the other cells, and on panels 5 and 6, the S edges
     * of the cells of all but the bottom row then the E edges of the cells of all but the last column.
     */
    static Panels panels(size_t n);

private:

    // -- Members

    const double equatorial_latitude_;
    const PointLonLat target_south_pole_;

    // -- Overridden methods

    void fill_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::grid::cubedsphere
