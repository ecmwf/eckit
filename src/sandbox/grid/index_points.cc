#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

#include "atlas/Gmsh.hpp"
#include "atlas/Mesh.hpp"
#include "atlas/MeshGen.hpp"
#include "atlas/FunctionSpace.hpp"
#include "atlas/Field.hpp"
#include "atlas/Parameters.hpp"

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

#define NLATS 4
#define NLONG 4

//------------------------------------------------------------------------------------------------------

int main()
{    
    typedef std::numeric_limits< double > dbl;
    std::cout.precision(dbl::digits10);
    std::cout << std::scientific;

//    std::vector< KPoint3 >* ipts = Tesselation::generate_latlon_points(NLATS, NLONG);
    std::unique_ptr< std::vector< KPoint3 > > ipts( Tesselation::generate_latlon_grid(NLATS, NLONG) );

    std::cout << "initial points " << ipts->size() << std::endl;

    PointSet points( *ipts );

    std::vector< KPoint3 > opts;
    std::vector< size_t >  idxs;

    points.list_unique_points(opts,idxs);

    std::cout << "duplicates  points " << points.duplicates().size() << std::endl;
    std::cout << "unique  points " << opts.size() << std::endl;

    // print indexes of unique points

    for( size_t i = 0; i < opts.size(); ++i )
    {
        std::cout << points.unique( opts[i], std::numeric_limits<size_t>::max() ) << std::endl;
    }

    return 0;
}
