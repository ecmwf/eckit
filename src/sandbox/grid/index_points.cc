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

class PointSet {

    typedef std::map< size_t, size_t > DupStore_t;

public:

    PointSet( std::vector< Point3 >* ipts )
    {
        reset();

        // std::cout << "building kdtree ... " << std::endl;

        npts_ = ipts->size();

        std::vector< PointIndex3::Value > pidx;
        pidx.reserve(npts_);

        for( size_t ip = 0; ip < npts_; ++ip )
        {
            Point3& p = (*ipts)[ip]; /* std::cout << p << std::endl; */
            pidx.push_back( PointIndex3::Value( PointIndex3::Point(p), ip ) );
        }

        PathName path (std::string("~/tmp/cache/grid/") + "test" + ".kdtree");
        PathName tmp  (std::string("~/tmp/cache/grid/") + "test" + ".tmp");
        tmp.unlink();

        tree_ = new PointIndex3(tmp, npts_, 0);

        tree_->build(pidx.begin(), pidx.end());

        PathName::rename(tmp, path);
    }

    ~PointSet(){ delete tree_; }

    std::vector< KPoint3 >* list_unique_points()
    {
        std::vector< KPoint3 >* opts = new std::vector< KPoint3 >( );
        opts->reserve(npts_);

        // std::cout << "computing duplicates ... " << std::endl;

        for( PointIndex3::iterator i = tree_->begin(); i != tree_->end(); ++i )
        {
            KPoint3 p (  i->point() );
            size_t  ip = i->payload();
            std::cout << "point " << ip << " " << p << std::endl;
            size_t uidx = unique(p,ip);
            if( ip == uidx )
            {
                std::cout << "----> UNIQ " << ip << std::endl;
                opts->push_back(p);
            }
            else
            {
                std::cout << "----> DUP " << ip << " -> " << uidx << std::endl;
            }
        }
        return opts;
    }

    size_t unique( eckit::KPoint3& p, size_t idx )
    {
        if( !n_ ) /* only for top-level call ( n_ == 0 ) */
        {
            DupStore_t::iterator dit = duplicates_.find(idx);
            if( dit != duplicates_.end() )
            {
//                std::cout << "      !! DUPLICATE !!" << std::endl;
                return dit->second;
            }
        }

//        DBGX(Kn());

        PointIndex3::NodeList nearest = tree_->kNearestNeighbours(p,Kn());

        std::vector<size_t> equals;
        equals.reserve( nearest.size() );

        for( size_t i = 0; i < nearest.size(); ++i )
        {
            KPoint3 np  = nearest[i].value().point();
            size_t nidx = nearest[i].value().payload();

//            std::cout << "      - " << nidx << " " << np << std::endl;

            if( points_equal(p,np) )
            {
//                std::cout << "      EQUAL !!" << std::endl;
                equals.push_back(nidx);
          }
            else
                break;
        }

//        DBGX( equals.size() );

        if( equals.size() == nearest.size() ) /* need to increase the search to find all duplicates of this point */
        {
            ++n_;
            return this->unique(p,idx);
        }
        else /* stop recursion */
        {
            size_t ret = idx; /* if nothing found return same idx */

            if( equals.size() >= 1 ) /* if an equal point was found return the first one */
                ret = equals[0];

            for( size_t n = 1; n < equals.size(); ++n )
                duplicates_[ equals[n] ] = ret;

            reset(); /* for next usage */

            return ret;
        }
    }

    DupStore_t& duplicates() { return duplicates_; }

protected:

    size_t Kn()
    {
        if( !n_ )
            return 2;
        else
            return 2 + std::pow(2,n_) * 180;
    }

    void reset() { n_ = 0; }
    bool duplicate( size_t idx ) { return duplicates_.find(idx) != duplicates_.end(); }

private:
    size_t n_;
    size_t npts_;
    PointIndex3* tree_;
    DupStore_t duplicates_; ///< map from duplicate idx to idx representing group of points
};

//------------------------------------------------------------------------------------------------------

int main()
{    
    typedef std::numeric_limits< double > dbl;
    std::cout.precision(dbl::digits10);
    std::cout << std::scientific;

//    std::vector< Point3 >* ipts = atlas::MeshGen::generate_latlon_points(NLATS, NLONG);
    std::vector< Point3 >* ipts = atlas::MeshGen::generate_latlon_grid(NLATS, NLONG);

    std::cout << "initial points " << ipts->size() << std::endl;

    PointSet points( ipts );

    std::vector< KPoint3 >* opts = points.list_unique_points();

    std::cout << "duplicates  points " << points.duplicates().size() << std::endl;
    std::cout << "unique  points " << opts->size() << std::endl;

    // print indexes of unique points

    for( size_t i = 0; i < opts->size(); ++i )
    {
        std::cout << points.unique( (*opts)[i], std::numeric_limits<size_t>::max() ) << std::endl;
    }

    // free memory

    delete ipts;
    delete opts;

    return 0;
}
