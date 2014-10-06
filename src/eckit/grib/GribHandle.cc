/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "grib_api.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/parser/StringTools.h"
#include "eckit/io/DataHandle.h"
#include "eckit/io/StdFile.h"
#include "eckit/geometry/Point2.h"

#include "eckit/grib/GribHandle.h"
#include "eckit/grib/GribAccessor.h"

using namespace std;

namespace eckit {
namespace grib {

//------------------------------------------------------------------------------------------------------

int grib_call(int code, const char *msg)
{
	if(code)
	{
		StrStream os;
		os << msg << ": " << grib_get_error_message(code) << StrStream::ends;
		throw Exception(std::string(os));
	}
	return code;
}

//------------------------------------------------------------------------------------------------------

GribHandle::GribHandle(const PathName& path) :
	handle_(NULL),
	owns_handle_(false)
{
	StdFile f(path);

	int err = 0;
	grib_handle* h = grib_handle_new_from_file(0,f,&err);
	if(err != 0)
	{
		StrStream os;
		os << "GribHandle() failed to build from path " << path << StrStream::ends;
		throw Exception(std::string(os), Here());
	}

	ASSERT(h);

	handle_ = h;
	owns_handle_ = true;
}

GribHandle::GribHandle(grib_handle* h) :
	handle_(NULL),
	owns_handle_(false)
{
	ASSERT(h);

	handle_ = h;
	owns_handle_ = true;
}

GribHandle::GribHandle(grib_handle& handle) :
	handle_(&handle),
	owns_handle_(false)
{
}

GribHandle::GribHandle(const Buffer& buffer, bool copy)
	: handle_(NULL),
	  owns_handle_(false)
{
	const char *message = buffer;
	ASSERT(strncmp(message,"GRIB", 4) == 0);

	grib_handle *h = 0;

	if(copy)
	{
		h = grib_handle_new_from_message_copy(0,const_cast<char*>(message),buffer.size());
	}
	else
	{
		h = grib_handle_new_from_message(0,const_cast<char*>(message),buffer.size());
	}

	ASSERT(h);

	handle_ = h;
	owns_handle_ = true;
}

GribHandle::~GribHandle()
{
	if( owns_handle_ && handle_ )
	{
		GRIB_CALL( grib_handle_delete(handle_) );
		handle_ = 0;
	}
}

std::string GribHandle::gridType() const
{
	return GribAccessor<std::string>("gridType")(*this);
}

std::string GribHandle::geographyHash() const
{
	return grib_geography_hash(*this);
}

long GribHandle::edition() const
{
	return GribAccessor<long>("edition")(*this);
}

size_t GribHandle::getDataValuesSize() const
{
    size_t count = 0;
	GRIB_CALL(grib_get_size(raw(), "values", &count));
    return count;
}

void GribHandle::getDataValues(double* values, const size_t& count) const
{
    ASSERT(values);
    size_t n = count;
	GRIB_CALL(grib_get_double_array(raw(),"values",values,&n));
	ASSERT(n == count);
}

void GribHandle::getLatLonPoints(std::vector<geometry::LLPoint2>& points) const
{
	size_t nb_nodes = nbDataPoints();

	points.resize(nb_nodes);

	/// It should be noted that grib iterator is *only* available for certain grids
	/// i.e for Spherical Harmonics it is not implemented.
	int err = 0;
	grib_iterator *i = grib_iterator_new( raw(), 0, &err );
	if ( err != 0 )
	{
	   throw SeriousBug(string("Error reading grib. Could not create grib_iterator_new"),Here()) ;
	}

	double lat   = 0.;
	double lon   = 0.;
	double value = 0.;

	size_t idx = 0;
	while( grib_iterator_next(i,&lat,&lon,&value) )
	{
	   points[idx].assign(lat,lon);
	   ++idx;
	}

	ASSERT( idx == nb_nodes );

	if ( grib_iterator_delete(i) != 0 )
	   throw SeriousBug(string("Error reading grib. Could not delete grib iterator"),Here()) ;
}

double* GribHandle::getDataValues(size_t& count) const
{
    count = getDataValuesSize();

    double* values = new double[count];
    getDataValues(values,count);
    return values;
}

void GribHandle::setDataValues(const double *values, size_t count)
{
    ASSERT(values);
	GRIB_CALL(grib_set_double_array(raw(),"values",values,count));
}

void GribHandle::write( DataHandle& handle )
{
    const void* message;
    size_t length;
	GRIB_CALL(grib_get_message(raw(), &message, &length));
    ASSERT(handle.write(message, length) == length);
}

size_t GribHandle::write( Buffer& buff )
{
    size_t len = buff.size();
	GRIB_CALL( grib_get_message_copy( raw(), buff, &len )); // will issue error if buffer too small
	return len;
}

double GribHandle::latitudeOfFirstGridPointInDegrees() const
{
	return GribAccessor<double>("latitudeOfFirstGridPointInDegrees")(*this);
}

double GribHandle::longitudeOfFirstGridPointInDegrees() const
{
	return GribAccessor<double>("longitudeOfFirstGridPointInDegrees")(*this);
}

double GribHandle::latitudeOfLastGridPointInDegrees() const
{
	return GribAccessor<double>("latitudeOfLastGridPointInDegrees")(*this);
}

double GribHandle::longitudeOfLastGridPointInDegrees() const
{
	return GribAccessor<double>("longitudeOfLastGridPointInDegrees")(*this);
}

GribHandle* GribHandle::clone() const
{
	grib_handle* h = grib_handle_clone(raw());
    if(!h)
        throw eckit::WriteError( std::string("failed to clone output grib") );

	return new GribHandle(h);
}

string GribHandle::shortName() const
{
	return GribAccessor<std::string>("shortName")(*this);
}

size_t GribHandle::nbDataPoints() const
{
	return GribAccessor<long>("numberOfDataPoints")(*this);
}

bool GribHandle::hasKey(const char* key) const
{
   if (grib_is_defined(handle_,key) == 0) return false;
   return true;
}

//------------------------------------------------------------------------------------------------------

} // namespace grib
} // namespace eckit
