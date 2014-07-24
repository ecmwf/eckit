/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/grib/GribAccessor.h"
#include "eckit/grib/GribHandle.h"


//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace grib {

//------------------------------------------------------------------------------------------------------

static GribAccessor<long> edition("edition");
static GribAccessor<std::string> md5Section2("md5Section2");
static GribAccessor<std::string> md5Section3("md5Section3");

std::string grib_geography_hash(grib_handle *h)
{
    ASSERT(h);

    /// @todo create a 'geographyMd5'  accessor

    std::string md5;

    switch( edition(h) )
    {
    case 1:
        md5 = md5Section2(h);
        break;
    case 2:
        md5 = md5Section3(h);
        break;

    default:
        ASSERT( !md5.empty() );
        break;
    }

    return md5;
}


std::string grib_geography_hash(const std::string &fname)
{
    FILE* fh = ::fopen( fname.c_str(), "r" );
    if( fh == 0 )
        throw ReadError( std::string("error opening file ") + fname );

    int err = 0;
    grib_handle* h = grib_handle_new_from_file(0,fh,&err);

    if( h == 0 || err != 0 )
        throw ReadError( std::string("error reading grib file ") + fname );

    std::string md5 = grib_geography_hash(h);

    grib_handle_delete(h);

    if( ::fclose(fh) == -1 )
        throw ReadError( std::string("error closing file ") + fname );

    return md5;
}

//------------------------------------------------------------------------------------------------------

} // namespace grib
} // namespace eckit
