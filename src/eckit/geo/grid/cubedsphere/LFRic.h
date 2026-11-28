// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0


#pragma once

#include <cstddef>
#include <string>

#include "eckit/geo/PointLonLat.h"
#include "eckit/geo/grid/CubedSphere.h"


namespace eckit::geo::grid::cubedsphere {


/**
 * @brief LFRic (Met Office) cubed sphere, numbered as LFRic's cubedsphere_mesh_generator (gencube_ps_mod)
 * @details Panels 'lfric': 1 to 4 band the equator eastwards (1 centred on (0, 0)), 5 and 6 on the North and South
 * Poles. Vertices can be stretched ('equatorial_latitude', Schmidt transform), then rotated as rotate_mesh_coords
 * ('target_south_pole', 'target_north_pole' or 'target_null_island', recorded as 'target_south_pole'). Names are
 * CS-LFR-<n>-C (also CS-LFR-<n>), CS-LFR-<n>-V and CS-LFR-<n>-E, case-insensitive, by hyphen or underscore.
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
     * cells (as calc_adjacency); vertices (as calc_face_to_vert) are the NW vertex of each cell of panels 1 to 4,
     * then the inner vertices of panel 5, then all vertices of panel 6; edges (as calc_edges) are, on panels 1 to 4,
     * the N, W and S edges of the cells of the top row then the W and S edges of the other cells, and on panels 5 and
     * 6, the S edges of the cells of all but the bottom row then the E edges of the cells of all but the last column.
     */
    static Panels panels(size_t n);

private:

    // -- Members

    const double equatorial_latitude_;
    const PointLonLat target_south_pole_;

    // -- Overridden methods

    bool fill_named_spec(spec::Custom&) const override;
};


}  // namespace eckit::geo::grid::cubedsphere
