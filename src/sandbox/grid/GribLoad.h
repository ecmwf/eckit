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

std::vector< KPoint3 >* read_ll_points_from_grib( const std::string& filename )
{
    int err = 0;

    // points to read

    std::vector< KPoint3 >* pts = new std::vector< KPoint3 >();

    // load grib file

    FILE* f = ::fopen( filename.c_str(), "r" );
    if( f == 0 )
        throw std::string("error opening file");

    grib_handle* h = grib_handle_new_from_file(0,f,&err);

    long nbDataPts = 0;
    grib_get_long(h,"numberOfDataPoints",&nbDataPts);

    pts->resize(nbDataPts);

    if( h == 0 || err != 0 )
        throw std::string("error reading grib");

    grib_iterator *i = grib_iterator_new(h, 0, &err);

    double lat   = 0.;
    double lon   = 0.;
    double value = 0.;

    /// we assume a row first scanning order on the grib
    size_t npoint = 0;
    while(grib_iterator_next(i,&lat,&lon,&value))
    {
        while(lon < 0)    lon += 360;
        while(lon >= 360) lon -= 360;

        atlas::latlon_to_3d( lat, lon, (*pts)[npoint].data() );
        ++npoint;
    }
    grib_iterator_delete(i);

    ASSERT( npoint == nbDataPts );

    if( ::fclose(f) == -1 )
        throw std::string("error closing file");

    #if 0
    for( int e = 0; e < pts->size(); ++e )
    {
        const KPoint3& p = (*pts)[e];
        std::cout <<  p(XX) << " "
                  <<  p(YY) << " "
                  <<  p(ZZ) << std::endl;
    }
    #endif

    return pts;
}

//------------------------------------------------------------------------------------------------------

template < typename FT >
void read_field_from_grib( const std::string& filename, FT& field )
{
    int err = 0;

    // load grib file

    FILE* f = ::fopen( filename.c_str(), "r" );
    if( f == 0 )
        throw std::string("error opening file");

    grib_handle* h = grib_handle_new_from_file(0,f,&err);

    if( h == 0 || err != 0 )
        throw std::string("error reading grib");

    long nbDataPts = 0;
    grib_get_long(h,"numberOfDataPoints",&nbDataPts);

    if( nbDataPts != field.size() )
    {
        std::ostringstream msg;
        msg << "number of data points in grib " << nbDataPts
            << " differs from field " << field.size()
            << std::endl;
        throw SeriousBug( msg.str() );
    }

    grib_iterator *i = grib_iterator_new(h, 0, &err);

    double lat   = 0.;
    double lon   = 0.;
    double value = 0.;

    size_t in = 0;
    while(grib_iterator_next(i,&lat,&lon,&value))
    {
        if( in >= field.size() )
            throw SeriousBug( "field is of incorrect size -- too many points" );
        field[in] = value;
        ++in;
    }
    grib_iterator_delete(i);

    if( in != field.size() )
        throw SeriousBug( "field is of incorrect size -- too little points" );

    if( ::fclose(f) == -1 )
        throw std::string("error closing file");

}

//------------------------------------------------------------------------------------------------------

atlas::FieldT<double>& read_field_into_mesh_from_grib( const std::string& filename, atlas::Mesh& mesh )
{
    atlas::FunctionSpace& nodes  = mesh.function_space( "nodes" );

    atlas::FieldT<double>& field = nodes.create_field<double>("field",1);

    read_field_from_grib( filename, field );

    return field;
}

//---------------------------------------------------------------------------------------------------------

} // namespace eckit

#endif

