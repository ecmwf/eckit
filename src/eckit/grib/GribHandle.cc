/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <grib_api.h>

#include "eckit/exception/Exceptions.h"
#include "eckit/parser/StringTools.h"
#include "eckit/io/DataHandle.h"

#include "eckit/grib/GribHandle.h"
#include "eckit/grib/GribAccessor.h"

namespace eckit {

static GribAccessor<long> grib_edition("edition");

GribHandle::GribHandle(grib_handle* handle)
    :handle_(handle)
{
    ASSERT(handle_);
}

GribHandle::GribHandle(const Buffer& buffer, bool copy)
    :handle_(0)
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
}

GribHandle::~GribHandle()
{
    ASSERT(handle_);
    grib_handle_delete(handle_);
}

long GribHandle::edition() const
{
    ASSERT(handle_);
    return grib_edition(handle_);
}

size_t GribHandle::getDataValuesSize() const
{
    ASSERT(handle_);
    size_t count = 0;
    GRIB_CALL(grib_get_size(handle_, "values", &count));
    return count;
}

void GribHandle::getDataValues(double* values, const size_t& count) const
{
    ASSERT(values);
    size_t n = count;
    GRIB_CALL(grib_get_double_array(handle_,"values",values,&n));
    ASSERT(n == count);
}

double* GribHandle::getDataValues(size_t& count) const
{
    ASSERT(handle_);

    count = getDataValuesSize();

    double* values = new double[count];
    getDataValues(values,count);
    return values;
}

void GribHandle::setDataValues(const double *values, size_t count)
{
    ASSERT(handle_);
    ASSERT(values);
    GRIB_CALL(grib_set_double_array(handle_,"values",values,count));
}

void GribHandle::write( DataHandle& handle )
{
    ASSERT(handle_);
    const void* message;
    size_t length;
    GRIB_CALL(grib_get_message(handle_, &message, &length));
    ASSERT(handle.write(message, length) == length);
}

size_t GribHandle::write( Buffer& buff )
{
    ASSERT(handle_);
    size_t len = buff.size();
    GRIB_CALL( grib_get_message_copy( handle_, buff, &len )); // will issue error if buffer too small
    return len;
}

GribHandle* GribHandle::clone() const
{
    ASSERT(handle_);
    grib_handle* h = grib_handle_clone(handle_);
    if(!h)
        throw eckit::WriteError( std::string("failed to clone output grib") );

    return new GribHandle(h);
}

size_t GribHandle::getNbDataPoints() const
{
    ASSERT(handle_);
    long npts;
    GRIB_CALL( grib_get_long( handle_,"numberOfDataPoints",&npts) );
    return npts;
}

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
} // namespace eckit
