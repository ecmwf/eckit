// (C) Copyright 1996-2014 ECMWF.

#ifndef eckit_Tesselation_hpp
#define eckit_Tesselation_hpp

#include <vector>

#include "PointIndex3.h"
#include "Tesselation.h"

namespace atlas { class Mesh; }

namespace eckit {

//------------------------------------------------------------------------------------------------------

class Tesselation {

public:

    /// generate a mesh by triangulating the convex hull of the 3D points
    static atlas::Mesh* generate_from_points( const std::vector<KPoint3>& pts );

    /// generate regular spaced lat-long points (does not include extremes)
    static std::vector< KPoint3 >* generate_latlon_points( size_t nlats, size_t nlong );

    /// generate regular lat-long grid points (includes extremes -90,90 and 0,360)
    static std::vector< KPoint3 >* generate_latlon_grid( size_t nlats, size_t nlong );

    /// generates the cell centres en each cell
    /// @warning only for triangles ATM
    /// @warning this function must be checked for the correct INDEX translation to Fortran
    static void create_cell_centres( atlas::Mesh& mesh );

};

//------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif // eckit_Tesselation_hpp
