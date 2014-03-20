// (C) Copyright 1996-2014 ECMWF.

#include <cmath>
#include <vector>
#include <memory>
#include <iostream>

#include "eckit/log/Timer.h"

#include <boost/progress.hpp>

#include "eckit/eckit_config.h"

//------------------------------------------------------------------------------------------------------

#ifdef CGAL_FOUND

// CGAL needs -DCGAL_NDEBUG to reach peak performance ...
#define CGAL_NDEBUG

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/algorithm.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/convex_hull_3.h>
#include <CGAL/Real_timer.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Polyhedron_3<K>                     Polyhedron_3;

typedef K::Vector_3                               Vector_3;
typedef K::FT                                     FT;
typedef K::Segment_3                              Segment_3;
typedef K::Point_3                                Point_3;

const Point_3 origin = Point_3(CGAL::ORIGIN);

#endif

//------------------------------------------------------------------------------------------------------

#include "atlas/Field.hpp"
#include "atlas/FunctionSpace.hpp"
#include "atlas/Mesh.hpp"
#include "atlas/Parameters.hpp"

#include "PointSet.h"
#include "Tesselation.h"

using namespace atlas;

namespace eckit {

//------------------------------------------------------------------------------------------------------

#ifdef CGAL_FOUND

Polyhedron_3* create_convex_hull_from_points( const std::vector< KPoint3 >& pts )
{
    Timer t("convex hull");

    Polyhedron_3* poly = new Polyhedron_3();

    // insertion from a vector :

    std::vector<Point_3> vertices( pts.size() );
    for( size_t i = 0; i < vertices.size(); ++i )
        vertices[i] = Point_3( pts[i](XX), pts[i](YY), pts[i](ZZ) );

    // compute convex hull of non-collinear points

    CGAL::convex_hull_3( vertices.begin(), vertices.end(), *poly );

    return poly;
}

atlas::Mesh* cgal_polyhedron_to_atlas_mesh( Polyhedron_3& poly, PointSet& points )
{
    Timer t ("creating atlas data structure");

    bool ensure_outward_normals = true;

    Mesh* mesh = new Mesh();

    /* nodes */

    const size_t nb_nodes = points.size();

    std::vector<int> bounds(2);
    bounds[0] = Field::UNDEF_VARS;
    bounds[1] = nb_nodes;

    FunctionSpace& nodes = mesh->add_function_space( new FunctionSpace( "nodes", "Lagrange_P0", bounds ) );

    nodes.metadata().set("type",static_cast<int>(Entity::NODES));

    FieldT<double>& coords  = nodes.create_field<double>("coordinates",3);

    FieldT<int>& glb_idx  = nodes.create_field<int>("glb_idx",1);

    std::cout << "inserting nodes (" << nb_nodes << ")" << std::endl;

//    boost::progress_display show_nodes_progress( nb_nodes );
    size_t inode = 0;
    for( PointSet::iterator it = points.begin(); it != points.end(); ++it )
    {
        KPoint3& p = it->point();
        size_t  ip = it->payload();

        glb_idx(ip) = ip;

        coords(XX,ip) = p(XX);
        coords(YY,ip) = p(YY);
        coords(ZZ,ip) = p(ZZ);

//        std::cout << p << std::endl;

        ++inode;
//        ++show_nodes_progress;
    }

    ASSERT( inode == nb_nodes ); /* check we went through all nodes */

    /* triangles */

    const size_t nb_triags = poly.size_of_facets();

    bounds[1] = nb_triags;

    FunctionSpace& triags  = mesh->add_function_space( new FunctionSpace( "triags", "Lagrange_P1", bounds ) );
    triags.metadata().set("type",static_cast<int>(Entity::ELEMS));

    FieldT<int>& triag_nodes = triags.create_field<int>("nodes",3);

    KPoint3 pt;
    size_t idx[3];
    Polyhedron_3::Vertex_const_handle vts[3];


    std::cout << "inserting triags (" << nb_triags << ")" << std::endl;

//    boost::progress_display show_triag_progress( nb_triags );
    size_t tidx = 0;
    for( Polyhedron_3::Facet_const_iterator f = poly.facets_begin(); f != poly.facets_end(); ++f )
    {

        // loop  over half-edges and take each vertex()

        size_t iedge = 0;
        Polyhedron_3::Halfedge_around_facet_const_circulator edge = f->facet_begin();
        do
        {
            Polyhedron_3::Vertex_const_handle vh = edge->vertex();
            const Polyhedron_3::Point_3& p = vh->point();

            pt.assign(p);

            idx[iedge] = points.unique( pt );

            ASSERT( idx[iedge] < nb_nodes );

            vts[iedge] = vh;

            ++iedge;
            ++edge;
        }
        while ( edge != f->facet_begin() && iedge < 3 );

        ASSERT( iedge == 3 );

        if( ensure_outward_normals ) /* ensure outward pointing normal */
        {
            Vector_3 p0 ( origin, vts[0]->point() );
            Vector_3 n  = CGAL::normal( vts[0]->point(), vts[1]->point(), vts[2]->point() );

            FT innerp = n * p0;

            if( innerp < 0 ) // need to swap an edge of the triag
                std::swap( vts[1], vts[2] );
        }

        /* define the triag */

        triag_nodes(0,tidx) = idx[0];
        triag_nodes(1,tidx) = idx[1];
        triag_nodes(2,tidx) = idx[2];

        ++tidx;
//        ++show_triag_progress;
    }

    assert( tidx == nb_triags );

    return mesh;
}

#endif

//------------------------------------------------------------------------------------------------------

atlas::Mesh* Tesselation::generate_from_points(const std::vector<KPoint3>& pts)
{
    Timer t ("grid tesselation");

    Mesh* mesh = 0;

    // remove duplicate points

    PointSet points( pts ); /* will remember each point index in readpts */

    std::vector< KPoint3 > ipts;
    std::vector< size_t > idxs;

    points.list_unique_points( ipts, idxs );

//    std::cout << "unique pts " << ipts.size() << std::endl;
//    std::cout << "duplicates " << points.duplicates().size() << std::endl;

#ifdef CGAL_FOUND

    // define polyhedron to hold convex hull

    Polyhedron_3* poly = create_convex_hull_from_points( ipts );

//    std::cout << "convex hull " << poly->size_of_vertices() << " vertices" << std::endl;

    assert( poly->size_of_vertices() == ipts.size() );

    mesh = cgal_polyhedron_to_atlas_mesh( *poly, points );

    delete poly;
#else

    throw std::string( "CGAL package not found -- triangulation is disabled" );

#endif

    return mesh;
}

//------------------------------------------------------------------------------------------------------

std::vector<KPoint3>* Tesselation::generate_latlon_points( const size_t& nlats, const size_t& nlong )
{
    // generate lat/long points

    const size_t npts = nlats * nlong;

    std::cout << "generating nlats (" << nlats << ") x  (" << nlong << ")" << " = " << npts << std::endl;

    std::vector< KPoint3 >* pts = new std::vector< KPoint3 >( npts );

    const double lat_inc = 180. / (double)nlats;
    const double lat_start = -90 + 0.5*lat_inc;
//    const double lat_end   = 90. - 0.5*lat_inc;

    const double lon_inc = 360. / (double)nlong;
    const double lon_start = 0.5*lon_inc;
//    const double lon_end   = 360. - 0.5*lon_inc;

    double lat;
    double lon;

    size_t visits = 0;

    lon = lon_start;
    for( size_t jlon = 0 ; jlon < nlong; ++jlon, lon += lon_inc )
    {
        lat = lat_start;
        for( size_t ilat = 0; ilat < nlats; ++ilat, lat += lat_inc )
        {
            const size_t idx = ilat + ( jlon * nlats );

            ASSERT( idx < npts );

            KPoint3& p = (*pts)[ idx ];

            atlas::latlon_to_3d( lat, lon, p.data() );

//            std::cout << idx << " [ " << lat << " ; " << lon << " ] " << p << std::endl;

            ++visits;
        }
    }

    ASSERT( visits == npts );

    return pts;
}

//------------------------------------------------------------------------------------------------------

std::vector<KPoint3>* Tesselation::generate_latlon_grid(const size_t& nlats, const size_t& nlong)
{
    // generate lat/long points

    const size_t npts = (nlats+1) * (nlong+1);

    std::vector< KPoint3 >* pts = new std::vector< KPoint3 >( npts );

    const double lat_inc = 180. / nlats;
    const double lat_start = -90.;
    const double lat_end   =  90.;

    const double lon_inc = 360. / nlong;
    const double lon_start = 0.0;
    const double lon_end   = 360.;

    double lat;
    double lon;

    lon = lon_start;
    for( size_t jlon = 0 ; jlon < nlong+1; ++jlon, lon += lon_inc )
    {
        lat = lat_start;
        for( size_t ilat = 0; ilat < nlats+1; ++ilat, lat += lat_inc )
        {
            const size_t idx = ilat + ( jlon * (nlats+1) );

            assert( idx < npts );

            atlas::latlon_to_3d( lat, lon, (*pts)[ idx ].data() );

//            std::cout << idx << " "
//                      << lat << " "
//                      << lon << " "
//                      << (*pts)[ ilat*(nlats+1) + jlon ](XX) << " "
//                      << (*pts)[ ilat*(nlats+1) + jlon ](YY) << " "
//                      << (*pts)[ ilat*(nlats+1) + jlon ](ZZ) << " "
//                      << std::endl;
//            std::cout << (*pts)[idx] << std::endl;

            if( ilat == nlats ) lat = lat_end;
        }

        if( jlon == nlong ) lon = lon_end;
    }

    return pts;
}

//------------------------------------------------------------------------------------------------------

void Tesselation::create_cell_centres(Mesh &mesh)
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
        const int i0 =  triag_nodes(0,e);
        const int i1 =  triag_nodes(1,e);
        const int i2 =  triag_nodes(2,e);

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

} // namespace eckit

