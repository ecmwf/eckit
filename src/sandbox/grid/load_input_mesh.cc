#include <limits>
#include <cassert>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <memory>

#include "atlas/Gmsh.hpp"
#include "atlas/Mesh.hpp"

#include "GribLoad.h"
#include "PointIndex3.h"
#include "PointSet.h"
#include "Tesselation.h"

//------------------------------------------------------------------------------------------------------

#if 1
#define DBG     std::cout << Here() << std::endl;
#define DBGX(x) std::cout << #x << " -> " << x << std::endl;
#else
#define DBG
#define DBGX(x)
#endif

//------------------------------------------------------------------------------------------------------

using namespace atlas;
using namespace eckit;

//------------------------------------------------------------------------------------------------------

int main()
{    
    typedef std::numeric_limits< double > dbl;
    std::cout.precision(dbl::digits10);
    std::cout << std::fixed;

    // input grid

    std::cout << "> reading input points ..." << std::endl;

    std::unique_ptr< std::vector< KPoint3 > > readpts( read_ll_points_from_grib( "data.grib" ) );

    // generate mesh ...

    std::cout << "> generating tesselation ..." << std::endl;

    std::unique_ptr<Mesh> mesh( Tesselation::generate_from_points( *readpts ) );

    // read input field

    std::cout << "> reading input field ..." << std::endl;

    read_field_into_mesh_from_grib( "data.grib", *mesh );

    // output input mesh

    std::cout << "> output input mesh ..." << std::endl;

    atlas::Gmsh::write3dsurf(*mesh, std::string("in.msh") );
}
