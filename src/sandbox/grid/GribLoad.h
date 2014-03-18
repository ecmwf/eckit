#ifndef eckit_GribLoad_h
#define eckit_GribLoad_h

#include "grib_api.h"

#include "atlas/Mesh.hpp"
#include "atlas/MeshGen.hpp"
#include "atlas/FunctionSpace.hpp"
#include "atlas/Field.hpp"
#include "atlas/Parameters.hpp"

#include "PointIndex3.h"

//-----------------------------------------------------------------------------

namespace eckit {

//------------------------------------------------------------------------------------------------------

std::vector< atlas::Point3 >* read_ll_points_from_grib( const std::string& filename )
{
    int err = 0;

    // points to read

    std::vector< atlas::Point3 >* pts = new std::vector< atlas::Point3 >();

    // load grib file

    FILE* f = ::fopen( filename.c_str(), "r" );
    if( f == 0 )
        throw std::string("error opening file");

    grib_handle* h = grib_handle_new_from_file(0,f,&err);

    long nbDataPts = 0;
    grib_get_long(h,"numberOfDataPoints",&nbDataPts);

    pts->reserve(nbDataPts);

    if( h == 0 || err != 0 )
        throw std::string("error reading grib");

    grib_iterator *i = grib_iterator_new(h, 0, &err);

    double lat   = 0.;
    double lon   = 0.;
    double value = 0.;

    /// we assume a row first scanning order on the grib

    atlas::Point3 prev;
    while(grib_iterator_next(i,&lat,&lon,&value))
    {
        while(lon < 0)    lon += 360;
        while(lon >= 360) lon -= 360;

        pts->push_back( atlas::Point3() );

        atlas::latlon_to_3d( lat, lon, pts->back().data() );

        if( points_equal( prev, pts->back() ) )
        {
            pts->pop_back();
            continue;
        }

        prev = pts->back();
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

atlas::FieldT<double>& read_field_from_grib( const std::string& filename, atlas::Mesh* mesh )
{
    atlas::FunctionSpace& nodes  = mesh->function_space( "nodes" );

    const size_t nb_nodes = nodes.bounds()[1];

    atlas::FieldT<double>& field = nodes.create_field<double>("field",1);

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

//---------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif

