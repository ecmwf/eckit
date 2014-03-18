#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <set>
#include <sstream>
#include <vector>

#define EIGEN_NO_AUTOMATIC_RESIZING
#define EIGEN_DONT_ALIGN
#define EIGEN_DONT_VECTORIZE

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Sparse>

#include "grib_api.h"

#include "atlas/Gmsh.hpp"
#include "atlas/Mesh.hpp"
#include "atlas/MeshGen.hpp"
#include "atlas/FunctionSpace.hpp"
#include "atlas/Field.hpp"
#include "atlas/Parameters.hpp"

#include "PointIndex3.h"
#include "FloatCompare.h"

//------------------------------------------------------------------------------------------------------

#if 1
#define DBG     std::cout << Here() << std::endl;
#define DBGX(x) std::cout << #x << " -> " << x << std::endl;
#else
#define DBG
#define DBGX(x)
#endif

//------------------------------------------------------------------------------------------------------

using namespace Eigen;
using namespace atlas;
using namespace eckit;

//------------------------------------------------------------------------------------------------------

#define NLATS 4
#define NLONG 4

//------------------------------------------------------------------------------------------------------

class UniquePoint {

public:

    UniquePoint( PointIndex3& tree ) : tree_(tree) { reset(); }

    bool check( eckit::KPoint3& p, size_t idx )
    {
        if( !n_ && seen(idx) ) return false; /* only for top-level call ( n_ == 0 ) */

        DBGX(Kn());

        PointIndex3::NodeList nearest = tree_.kNearestNeighbours(p,Kn());

        size_t nequals = 0;
        for( size_t i = 0; i < nearest.size(); ++i )
        {
            KPoint3 np  = nearest[i].value().point();
            size_t nidx = nearest[i].value().payload();

            std::cout << "      - " << nidx << " " << p << std::endl;

            if( seen(nidx) ) { nequals++; continue; }

            if( points_equal(p,np) )
            {
                nequals++;
                seenIdx_.insert(idx);
            }
            else
                break;
        }

        DBGX( nequals );

        if( nequals == nearest.size() )
        {
            ++n_;
            return this->check(p,idx);
        }
        else /* stop recursion */
        {
            reset();
            return (nequals == 0);
        }
    }

    size_t seenSize() { return seenIdx_.size(); }

protected:

    size_t Kn()
    {
        if( !n_ )
            return 2;
        else
            return 2 + std::pow(2,n_) * 180;
    }

    void reset() { n_ = 0; }
    bool seen( size_t idx ) { return seenIdx_.find(idx) != seenIdx_.end(); }

private:
    size_t n_;
    PointIndex3& tree_;
    std::set< size_t > seenIdx_;
};

//------------------------------------------------------------------------------------------------------

int main()
{    
    typedef std::numeric_limits< double > dbl;
    std::cout.precision(dbl::digits10);
    std::cout << std::scientific;

    std::vector< Point3 >* ipts = atlas::MeshGen::generate_latlon_points(NLATS, NLONG);
    const size_t npts = ipts->size();

    std::cout << "initial points " << npts << std::endl;

    std::vector< KPoint3 > opts;
    opts.reserve(npts);

    /// BUILD KD-TREE ////////////////////////////////////////////////////////////

    std::cout << "building kdtree ... " << std::endl;

    std::vector< PointIndex3::Value > pidx;
    pidx.reserve(npts);

    for( size_t ip = 0; ip < npts; ++ip )
    {
        Point3& p = (*ipts)[ip];

        std::cout << p << std::endl;

        pidx.push_back( PointIndex3::Value( PointIndex3::Point(p), ip ) );
    }

    PathName path (std::string("~/tmp/cache/grid/") + "test" + ".kdtree");
    PathName tmp  (std::string("~/tmp/cache/grid/") + "test" + ".tmp");
    tmp.unlink();

    PointIndex3* tree = new PointIndex3(tmp, npts, 0);

    tree->build(pidx.begin(), pidx.end());

    PathName::rename(tmp, path);

    /// SEARCH ////////////////////////////////////////////////////////////

    UniquePoint uniq( *tree );

    for( size_t ip = 0; ip < ipts->size(); ++ip )
    {
        KPoint3 p (  (*ipts)[ip] );   std::cout << "point " << ip << " " << p << std::endl;

        if( uniq.check(p,ip) )
        {
            std::cout << "----> UNIQ" << std::endl;

            opts.push_back(p);
        }
    }

    std::cout << "duplicates  points " << uniq.seenSize() << std::endl;
    std::cout << "unique  points " << opts.size() << std::endl;

    delete ipts;
    delete tree;

    return 0;
}
