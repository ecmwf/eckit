#include <cassert>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <memory>

#include "grib_api.h"

#include "atlas/Gmsh.hpp"
#include "atlas/Mesh.hpp"
#include "atlas/MeshGen.hpp"
#include "atlas/FunctionSpace.hpp"
#include "atlas/Field.hpp"
#include "atlas/Parameters.hpp"

#include "eckit/container/KDTree.h"
#include "eckit/container/KDMapped.h"
#include "eckit/container/KDMemory.h"
#include "eckit/container/sptree/SPPoint.h"

//------------------------------------------------------------------------------------------------------

using namespace atlas;
using namespace eckit;

//------------------------------------------------------------------------------------------------------

class KPoint3 : public SPPoint<3> {
public:

    KPoint3(): SPPoint<3>() {}

    KPoint3( const double lat, const double lon ): SPPoint<3>()
    {
        atlas::latlon_to_3d( lat, lon, x_ );
    }

    KPoint3( const double x, const double y, const double z ): SPPoint<3>(x,y)
    {
        x_[ZZ] = z;
    }

};

//------------------------------------------------------------------------------------------------------

template<class Traits>
class PointKdTree : public eckit::KDTreeMapped<Traits> {
public:
    typedef eckit::KDTreeMapped<Traits> Tree;
    typedef typename Tree::Point Point;
    typedef typename Tree::Alloc    Alloc;

    typedef typename Tree::NodeList    NodeList;
    typedef typename Tree::NodeInfo    NodeInfo;
    typedef typename Tree::PayloadType Payload;
    typedef typename Tree::Value   Value;

    PointKdTree(const eckit::PathName& path,  size_t itemCount, size_t metadataSize):
        eckit::KDTreeMapped<Traits>(path, itemCount, metadataSize) {}

    NodeInfo nearestNeighbour(const Point& p)
    {
        return Tree::nearestNeighbour(p);
    }

    NodeInfo nearestNeighbourBruteForce(const Point& p)
    {
        return Tree::nearestNeighbourBruteForce(p);
    }

    NodeList findInSphere(const Point& p,double radius)
    {
        return Tree::findInSphere(p, radius);
    }

    NodeList findInSphereBruteForce(const Point& p,double radius)
    {
        return Tree::findInSphereBruteForce(p,radius);
    }

    NodeList kNearestNeighbours(const Point& p,size_t k)
    {
        return Tree::kNearestNeighbours(p, k);
    }

    NodeList kNearestNeighboursBruteForce(const Point& p,size_t k)
    {
        return Tree::kNearestNeighboursBruteForce(p,k);
    }

    PointKdTree(Alloc& alloc): Tree(alloc) {}
};

//------------------------------------------------------------------------------------------------------

struct CellCentreIndexTreeTrait
{
    typedef KPoint3   Point;
    typedef size_t    Payload;
};

//------------------------------------------------------------------------------------------------------

typedef PointKdTree<CellCentreIndexTreeTrait>  CellCentreIndex;

//------------------------------------------------------------------------------------------------------

std::vector< Point3 >* read_ll_points_from_grib( const std::string& filename )
{
    int err = 0;

    // points to read

    std::vector< Point3 >* pts = new std::vector< Point3 >();

    // load grib file

    FILE* f = ::fopen( filename.c_str(), "r" );
    if( f == 0 )
        throw std::string("error opening file");

    grib_handle* h = grib_handle_new_from_file(0,f,&err);

    if( h == 0 || err != 0 )
        throw std::string("error reading grib");

    grib_iterator *i = grib_iterator_new(h, 0, &err);

    double lat   = 0.;
    double lon   = 0.;
    double value = 0.;

    while(grib_iterator_next(i,&lat,&lon,&value))
    {
        while(lon < 0)    lon += 360;
        while(lon >= 360) lon -= 360;

        pts->push_back( Point3() );

        atlas::latlon_to_3d( lat, lon, pts->back().x );
    }
    grib_iterator_delete(i);

    if( ::fclose(f) == -1 )
        throw std::string("error closing file");

    return pts;
}

//------------------------------------------------------------------------------------------------------

void create_cell_centres( atlas::Mesh& mesh )
{
    FunctionSpace& nodes     = mesh.function_space( "nodes" );
    FieldT<double>& coords   = nodes.field<double>( "coordinates" );

    const size_t nb_nodes = nodes.bounds()[1];

    FunctionSpace& triags      = mesh.function_space( "triags" );
    FieldT<int>& triag_nodes   = triags.field<int>( "nodes" );

    const size_t nb_triags = triags.bounds()[1];

    FieldT<double>& triags_centres   = triags.create_field<double>("centre",3);

    const double third = 1. / 3.;
    for( int e = 0; e < nb_triags; ++e )
    {
        const int i0 =  C_IDX( triag_nodes(0,e) );
        const int i1 =  C_IDX( triag_nodes(1,e) );
        const int i2 =  C_IDX( triag_nodes(2,e) );

        assert( i0 < nb_nodes && i1 < nb_nodes && i2 < nb_nodes );

//        std::cout << i0 << " " << i1 << " " << i2 << std::endl;
//        for( int i = 0; i < 3; ++i )
//           std::cout <<  coords(XX,C_IDX( triag_nodes(i,e) )) << " "
//                     <<  coords(YY,C_IDX( triag_nodes(i,e) )) << " "
//                     <<  coords(ZZ,C_IDX( triag_nodes(i,e) )) << std::endl;

        triags_centres(XX,e) = third * ( coords(XX,i0) + coords(XX,i1) + coords(XX,i2) );
        triags_centres(YY,e) = third * ( coords(YY,i0) + coords(YY,i1) + coords(YY,i2) );
        triags_centres(ZZ,e) = third * ( coords(ZZ,i0) + coords(ZZ,i1) + coords(ZZ,i2) );
    }
}

//------------------------------------------------------------------------------------------------------

template < typename Tree >
Tree* create_point_index( atlas::Mesh& mesh )
{
    FunctionSpace& triags = mesh.function_space( "triags" );
    FieldT<double>& triags_centres = triags.field<double>( "centre" );

    const size_t npts = triags.bounds()[1];

    PathName path(std::string("~/tmp/cache/grid/") + "MD5" + ".kdtree");

    std::vector<typename Tree::Value> p;
    p.reserve(npts);

    for( size_t ip = 0; ip < npts; ++ip )
    {
        p.push_back( typename Tree::Value( typename Tree::Point( triags_centres(XX,ip),
                                                                 triags_centres(YY,ip),
                                                                 triags_centres(ZZ,ip) ), ip ) );
    }

    PathName tmp(std::string("~/tmp/cache/grid/") + "MD5" + ".tmp");
    tmp.unlink();

    Tree* tree = new Tree(tmp, npts, 0);

    tree->build(p.begin(), p.end());

    PathName::rename(tmp, path);

    return tree;
}

//------------------------------------------------------------------------------------------------------

int main()
{
    std::vector< Point3 >* pts;

    std::cout << "> loading data.grib ..." << std::endl;

    pts = read_ll_points_from_grib( "data.grib" );

    std::cout << "> generating mesh ..." << std::endl;

    Mesh* mesh = atlas::MeshGen::generate_from_points( *pts );

    delete pts;

    // generate baricenters of each triangle

    std::cout << "> generating cell centres ..." << std::endl;

    create_cell_centres( *mesh );

    // insert the baricenters on a kd-tree

    std::cout << "> creating point index ..." << std::endl;

    CellCentreIndex* tree = create_point_index<CellCentreIndex>( *mesh );

    // for each point in output grid

        // search the nearest 4 triangles


        // find in which triangle the point is contained
        // by computing the intercetion of each point with triang;e


        // compute the weights

        // assemble the global (sparse) interpolant matrix


    delete mesh;

    return 0;
}
