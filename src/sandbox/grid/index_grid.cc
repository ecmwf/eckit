#include <limits>
#include <cassert>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <memory>

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

#include "eckit/container/KDTree.h"
#include "eckit/container/KDMapped.h"
#include "eckit/container/KDMemory.h"
#include "eckit/container/sptree/SPPoint.h"

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

class KPoint3 : public SPPoint<3> {
public:

    KPoint3(): SPPoint<3>() {}

    double* data() { return x_; }

    KPoint3( const Point3& p ): SPPoint<3>()
    {
        std::copy(p.x, p.x+3, x_);
    }

    KPoint3( const double lat, const double lon ): SPPoint<3>()
    {
        atlas::latlon_to_3d( lat, lon, x_ );
    }

    KPoint3( const double x, const double y, const double z ): SPPoint<3>(x,y)
    {
        x_[ZZ] = z;
    }

    friend std::ostream& operator<<(std::ostream& s,const KPoint3& p)
    {
        s << '(' << p.x_[XX] << "," << p.x_[YY] << ","  << p.x_[YY] << ')';
        return s;
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

#if 0
    for( int e = 0; e < pts->size(); ++e )
    {
        const Point3& p = (*pts)[e];
        std::cout <<  p(XX) << " "
                  <<  p(YY) << " "
                  <<  p(ZZ) << std::endl;
    }
#endif

    return pts;
}

//------------------------------------------------------------------------------------------------------

FieldT<double>& read_field_from_grib( const std::string& filename, atlas::Mesh* mesh )
{
    FunctionSpace& nodes     = mesh->function_space( "nodes" );

    const size_t nb_nodes = nodes.bounds()[1];

    FieldT<double>& field = nodes.create_field<double>("field",1);

    int err = 0;

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

    size_t in = 0;
    while(grib_iterator_next(i,&lat,&lon,&value))
    {
        if( in >= nb_nodes )
            throw SeriousBug( "field is of incorrect size" );
        field[in] = value;
        ++in;
    }
    grib_iterator_delete(i);

    if( in != nb_nodes )
        throw SeriousBug( "field is of incorrect size" );

    if( ::fclose(f) == -1 )
        throw std::string("error closing file");

    return field;
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

#if 0 /* print triangle connectivity */
        std::cout << i0 << " " << i1 << " " << i2 << std::endl;
#endif
#if 0 /* print triangle idx and coordinates */
           std::cout << e << " "
                     << i0 << " " << i1 << " " << i2 << " ";
           for( int i = 0; i < 3; ++i )
               std::cout << "("
                     <<  coords(XX,C_IDX( triag_nodes(i,e) )) << "; "
                     <<  coords(YY,C_IDX( triag_nodes(i,e) )) << "; "
                     <<  coords(ZZ,C_IDX( triag_nodes(i,e) )) << ")";
          std::cout << std::endl;
#endif
        triags_centres(XX,e) = third * ( coords(XX,i0) + coords(XX,i1) + coords(XX,i2) );
        triags_centres(YY,e) = third * ( coords(YY,i0) + coords(YY,i1) + coords(YY,i2) );
        triags_centres(ZZ,e) = third * ( coords(ZZ,i0) + coords(ZZ,i1) + coords(ZZ,i2) );

#if 0 /* print sorted triangle connectivity */
        std::vector<int> s;
        s.push_back(i0);
        s.push_back(i1);
        s.push_back(i2);
        std::sort(s.begin(),s.end());
        std::cout << s[0] << " " << s[1] << " " << s[2] << std::endl;
#endif
    }

#if 0 /* print triangle baricentres */
    for( int e = 0; e < nb_triags; ++e )
    {
        std::cout << triags_centres(XX,e) << " "
                  << triags_centres(YY,e) << " "
                  << triags_centres(ZZ,e) << " "
                  << e << " "
                  << std::endl;
    }
#endif
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

/// Intersection data structure

struct Isect
{
    double u;
    double v;
    double t;

    double w() const { return 1.0 - u - v; }

    friend std::ostream& operator<<(std::ostream& s,const Isect& p)
    {
        s << '(' << p.u << "," << p.v << ","  << p.w() << ","  << p.t << ')';
        return s;
    }

};

/// Ray trace data structure

struct Ray
{
    Vector3d orig;
    Vector3d dir;

    /// initializes ray with origin in point and direction to (0,0,0)
    Ray( double* p )
    {
        orig = Vector3d::Map(p);
        dir  = - orig;
    }

    Ray( double* o, double* d )
    {
        orig = Vector3d::Map(o);
        dir  = Vector3d::Map(d);
    }

    Vector3d operator()( double t ) const { return orig + t*dir; }
};

/// triangle structure

struct Triag
{
    Triag( double* x0, double* x1, double* x2 )
    {
        v0 = Vector3d::Map(x0);
        v1 = Vector3d::Map(x1);
        v2 = Vector3d::Map(x2);
    }

    Vector3d v0;
    Vector3d v1;
    Vector3d v2;
};

//------------------------------------------------------------------------------------------------------

const bool isSingledSided = false;
const double epsilon = std::numeric_limits<double>::epsilon();

bool triag_intersection( const Triag& tg, const Ray& r, Isect& isect )
{
    Vector3d edge1 = tg.v1 - tg.v0;
    Vector3d edge2 = tg.v2 - tg.v0;
    Vector3d pvec  = r.dir.cross(edge2);
    double det = edge1.dot(pvec);
    if( std::abs(det) < epsilon ) return false;
    double invDet = 1. / det;
    Vector3d tvec = r.orig - tg.v0;
    isect.u = tvec.dot(pvec) * invDet;
    if (isect.u < 0 || isect.u > 1) return false;
    Vector3d qvec = tvec.cross(edge1);
    isect.v = r.dir.dot(qvec) * invDet;
    if (isect.v < 0 || isect.u + isect.v > 1) return false;
    isect.t = edge2.dot(qvec) * invDet;
    return true;
}

//------------------------------------------------------------------------------------------------------

#define NLATS 256
#define NLONG 256

int main()
{    
    typedef std::numeric_limits< double > dbl;
    std::cout.precision(dbl::digits10);
    std::cout << std::fixed << std::endl;

    // output grid

    std::vector< Point3 >* opts = atlas::MeshGen::generate_latlon_points(NLATS, NLONG);

    // input grid

    std::cout << "> reading input points ..." << std::endl;

    std::vector< Point3 >* ipts = read_ll_points_from_grib( "data.grib" );

    // sparse interpolation matrix

    const size_t out_npts = opts->size();
    const size_t inp_npts = ipts->size();

    Eigen::SparseMatrix<double> W( out_npts, inp_npts );

    std::vector< Eigen::Triplet<double> > weights_entries; /* structure to fill-in sparse matrix */

    weights_entries.reserve( out_npts * 3 ); /* each row has 3 entries: one per vertice of triangle */

    // generate mesh ...

    std::cout << "> computing tesselation ..." << std::endl;

    Mesh* mesh = atlas::MeshGen::generate_from_points( *ipts );

    delete ipts;

    // read the field data

    std::cout << "> reading input field ..." << std::endl;

    FieldT<double>& inField = read_field_from_grib( "data.grib", mesh );

    // generate baricenters of each triangle & insert the baricenters on a kd-tree

    std::cout << "> creating triangle index ..." << std::endl;

    create_cell_centres( *mesh );

    CellCentreIndex* tree = create_point_index<CellCentreIndex>( *mesh );

    // compute weights for each point in output grid

    std::cout << "> computing weights ..." << std::endl;

    const size_t k = 4; /* search nearest k cell centres */

    for( size_t ip = 0; ip < opts->size(); ++ip )
    {
        KPoint3 p( (*opts)[ip] ); // lookup point
#if 0
        std::cout << p << std::endl;
#endif

        // search the nearest 4 triangles

        CellCentreIndex::NodeList cs = tree->kNearestNeighbours(p,k);

#if 0
        for( size_t i = 0; i < cs.size(); ++i )
        {
            std::cout << cs[i] << std::endl;
        }
#endif

        // find in which triangle the point is contained
        // by computing the intercetion of the point with each nearest triangle

        FunctionSpace& nodes     = mesh->function_space( "nodes" );
        FieldT<double>& coords   = nodes.field<double>( "coordinates" );

        const size_t nb_nodes = nodes.bounds()[1];

        FunctionSpace& triags      = mesh->function_space( "triags" );
        FieldT<int>& triag_nodes   = triags.field<int>( "nodes" );

        const size_t nb_triags = triags.bounds()[1];

        int idx[3]; /* indexes of the triangle that will contain the point*/

        Vector3d phi;
        Isect uvt;
        Ray ray( p.data() );

        size_t tid = std::numeric_limits<size_t>::max();

        bool found = false;
        for( size_t i = 0; i < cs.size(); ++i )
        {
            tid = cs[i].value().payload();

//            std::cout << "considering triag " << tid <<  std::endl;

            assert( tid < nb_triags );

            idx[0] = triag_nodes(0,tid);
            idx[1] = triag_nodes(1,tid);
            idx[2] = triag_nodes(2,tid);

            assert( idx[0] < nb_nodes && idx[1] < nb_nodes && idx[2] < nb_nodes );

            Triag triag( coords.slice(idx[0]), coords.slice(idx[1]), coords.slice(idx[2]) );

            if( triag_intersection( triag, ray, uvt ) )
            {
                found = true;
#if 0
                std::cout << " YES -- baricentric coords " << uvt <<  std::endl;
                std::cout << idx[0] << " " << idx[1] << " " << idx[2] << std::endl;
#endif
                // weights are the baricentric cooridnates u,v

                phi[0] = uvt.w();
                phi[1] = uvt.u;
                phi[2] = uvt.v;

                break;
            }
        }

        if( !found )
            throw SeriousBug("Could not project point into nearest triangles");

        // insert the interpolant weights into the global (sparse) interpolant matrix

        for(int i = 0; i < 3; ++i)
            weights_entries.push_back( Eigen::Triplet<double>( ip, idx[i], phi[i] ) );

    }

    atlas::Gmsh::write3dsurf(*mesh, std::string("in.msh") );

    // fill-in sparse matrix

    std::cout << "> filling matrix ..." << std::endl;

    W.setFromTriplets(weights_entries.begin(), weights_entries.end());

    // interpolation -- multiply interpolant matrix with field vector

    std::cout << "> interpolating ..." << std::endl;

    std::vector<double> result (out_npts); /* result vector */

    VectorXd f = VectorXd::Map( &(inField.data())[0], inField.data().size() );
    VectorXd r = VectorXd::Map( &result[0], result.size() );

    r = W * f;

    {
        atlas::Mesh* outMesh = MeshGen::generate_from_points(*opts);

        FunctionSpace& nodes     = outMesh->function_space( "nodes" );
        FieldT<double>& coords   = nodes.field<double>( "coordinates" );

        const size_t nb_nodes = nodes.bounds()[1];

        FieldT<double>& field = nodes.create_field<double>("field",1);

        for( size_t i = 0; i < out_npts; ++i )
            field[i] = r[i];

        atlas::Gmsh::write3dsurf(*outMesh, std::string("out.msh") );

        delete outMesh;
    }

    delete opts;
    delete mesh;

    return 0;
}
