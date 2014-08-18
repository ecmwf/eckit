/*
 * (C) Copyright 1996-2013 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/grib/Grib.h"
#include "eckit/grib/GribAccessor.h"
#include "eckit/grib/GribHandle.h"

//------------------------------------------------------------------------------------------------------

namespace eckit {
namespace grib {

//------------------------------------------------------------------------------------------------------

static void check_error_code( const std::string& name, int err, bool quiet = false )
{
	if(err && !quiet) {
	   eckit::Log::error() << "GribAccessor(" << name << "): " << grib_get_error_message(err) << std::endl;
	}
}

void grib_get_value(const GribHandle& h, const std::string& name, double& x, bool quiet)
{
	x = 0;
	int err = grib_get_double(h.raw(), name.c_str(), &x);
	check_error_code(name,err,quiet);
}

void grib_get_value(const GribHandle& h, const std::string& name, unsigned long& x, bool quiet)
{
	long y = 0;
	int err = grib_get_long(h.raw(), name.c_str(), &y);
	check_error_code(name,err,quiet);
	x = y;
}

void grib_get_value(const GribHandle& h, const std::string& name, long& x, bool quiet)
{
	x = 0;
	int err = grib_get_long(h.raw(), name.c_str(), &x);
	check_error_code(name,err,quiet);
}

void grib_get_value(const GribHandle& h, const std::string& name, std::string& x, bool quiet)
{
	char buf[1024];
	size_t s = sizeof(buf);
	buf[0] = 0;
	int err = grib_get_string(h.raw(), name.c_str(), buf, &s);
	check_error_code(name,err,quiet);
	x = buf;
}

void grib_get_value(const GribHandle& h, const std::string& name, std::vector<long>& x, bool quiet)
{
	int err = 0;
	size_t sz = 0;
	err = grib_get_size(h.raw(),name.c_str(),&sz); check_error_code(name,err,quiet);
	x.resize(sz);
	err = grib_get_long_array(h.raw(),name.c_str(),&x[0],&sz); check_error_code(name,err,quiet);
	ASSERT( x.size() == sz );
}

void grib_get_value(const GribHandle& h, const std::string& name, std::vector<double>& x, bool quiet)
{
	int err = 0;
	size_t sz = 0;
	err = grib_get_size(h.raw(),name.c_str(),&sz); check_error_code(name,err,quiet);
	x.resize(sz);
	err = grib_get_double_array(h.raw(),name.c_str(),&x[0],&sz); check_error_code(name,err,quiet);
	ASSERT( x.size() == sz );
}

//------------------------------------------------------------------------------------------------------

static GribAccessor<long> edition("edition");
static GribAccessor<std::string> md5Section2("md5Section2");
static GribAccessor<std::string> md5Section3("md5Section3");

std::string grib_geography_hash(const GribHandle& h)
{
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

	GribHandle gh(h);

	std::string md5 = grib_geography_hash(gh);

    if( ::fclose(fh) == -1 )
        throw ReadError( std::string("error closing file ") + fname );

    return md5;
}

//------------------------------------------------------------------------------------------------------

} // namespace grib
} // namespace eckit
