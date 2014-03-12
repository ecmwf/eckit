#include <limits>
#include <cassert>
#include <sstream>
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

#include "PointIndex3.h"
#include "TriangleIntersection.h"

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
    FunctionSpace& nodes  = mesh->function_space( "nodes" );

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

#define NLATS 256
#define NLONG 256

int main()
{    
    typedef std::numeric_limits< double > dbl;
    std::cout.precision(dbl::digits10);
    std::cout << std::fixed;

    // output grid

    std::vector< Point3 >* opts = atlas::MeshGen::generate_latlon_points(NLATS, NLONG);
//    atlas::Mesh* outMesh = MeshGen::generate_from_points(*opts);
//    atlas::Gmsh::write3dsurf(*outMesh, std::string("out.msh") );
//    delete outMesh;

    // input grid

    std::cout << "> reading input points ..." << std::endl;

    std::vector< Point3 >* ipts = read_ll_points_from_grib( "data.grib" );

//    for( size_t i = 0; i < ipts->size(); ++i )
//        std::cout << (*ipts)[i] << std::endl;

    // sparse interpolation matrix

    const size_t out_npts = opts->size();
    const size_t inp_npts = ipts->size();

    Eigen::SparseMatrix<double> W( out_npts, inp_npts );

    std::vector< Eigen::Triplet<double> > weights_triplets; /* structure to fill-in sparse matrix */

    // generate mesh ...

    std::cout << "> computing tesselation ..." << std::endl;

    Mesh* mesh = atlas::MeshGen::generate_from_points( *ipts );

    delete ipts;

    // read the field data

    std::cout << "> reading input field ..." << std::endl;

    FieldT<double>& inField = read_field_from_grib( "data.grib", mesh );

    // output input mesh

    std::cout << "> output input mesh ..." << std::endl;

    atlas::Gmsh::write3dsurf(*mesh, std::string("in.msh") );

    // generate baricenters of each triangle & insert the baricenters on a kd-tree

    std::cout << "> creating triangle index ..." << std::endl;

    atlas::MeshGen::create_cell_centres( *mesh );

    PointIndex3* tree = create_point_index<PointIndex3>( *mesh );

    // compute weights for each point in output grid

    std::cout << "> computing weights ..." << std::endl;

    FunctionSpace& nodes     = mesh->function_space( "nodes" );
    FieldT<double>& coords   = nodes.field<double>( "coordinates" );

    const size_t nb_nodes = nodes.bounds()[1];

    FunctionSpace& triags      = mesh->function_space( "triags" );
    FieldT<int>& triag_nodes   = triags.field<int>( "nodes" );

    const size_t nb_triags = triags.bounds()[1];

    //==== PONT LOOP =======================================================
    //==== PONT LOOP =======================================================
    //==== PONT LOOP =======================================================

    weights_triplets.reserve( out_npts * 3 ); /* each row has 3 entries: one per vertice of triangle */

    const size_t k = 4; /* search nearest k cell centres */

    for( size_t ip = 0; ip < opts->size(); ++ip )
    {
        std::ostringstream os;

        KPoint3 p( (*opts)[ip] ); // lookup point
#if 0
        std::cout << p << std::endl;
#endif

        // search the nearest 4 triangles

        PointIndex3::NodeList cs = tree->kNearestNeighbours(p,k);

#if 0
        for( size_t i = 0; i < cs.size(); ++i )
        {
            std::cout << cs[i] << std::endl;
        }
#endif

        // find in which triangle the point is contained
        // by computing the intercetion of the point with each nearest triangle

        int idx[3]; /* indexes of the triangle that will contain the point*/

        Vector3d phi;
        Isect uvt;
        Ray ray( p.data() );

        size_t tid = std::numeric_limits<size_t>::max();

        bool found = false;
        for( size_t i = 0; i < cs.size(); ++i )
        {
            tid = cs[i].value().payload();

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
            else
            {
                os << "FAILED projection on triangle:" << std::endl;
                os << "   " << tid << std::endl;
                os << "   " << idx[0] << " " << idx[1] << " " << idx[2] << std::endl;
                os << "   " << KPoint3(coords.slice(idx[0])) << " / "
                            << KPoint3(coords.slice(idx[1])) << " / "
                            << KPoint3(coords.slice(idx[2])) << std::endl;
                os << "   " << "Baricentre " << uvt << std::endl;
            }
        }

        if( !found )
        {
            std::cout << os.str() << std::endl;
            throw SeriousBug("Could not project point into nearest triangles");
        }

        // insert the interpolant weights into the global (sparse) interpolant matrix

        for(int i = 0; i < 3; ++i)
            weights_triplets.push_back( Eigen::Triplet<double>( ip, idx[i], phi[i] ) );

    }

#if SKIP
    // fill-in sparse matrix

    std::cout << "> filling matrix ..." << std::endl;

    W.setFromTriplets(weights_triplets.begin(), weights_triplets.end());

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
#endif

    delete opts;
    delete mesh;
    delete tree;

    return 0;
}
