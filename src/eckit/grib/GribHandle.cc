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
#include "GribHandle.h"

namespace eckit {

static int grib_call(int code, const char* msg) {
    if(code) {
        StrStream os;
        os << msg << ": " << grib_get_error_message(code) << StrStream::ends;
        throw Exception(std::string(os));
    }
    return code;
}


#define GRIB_CALL(a) grib_call(a, #a)

GribHandle::GribHandle(grib_handle* handle)
    :handle_(handle)
{

}

GribHandle::GribHandle(const Buffer& buffer,size_t size, bool copy)
    :handle_(0)
{
    const char *message = buffer;
    ASSERT(strncmp(message,"GRIB", 4) == 0);
    grib_handle *h = 0;
    if(copy) {
        h = grib_handle_new_from_message_copy(0,const_cast<char*>(message),size);
    }
    else {
        h = grib_handle_new_from_message(0,const_cast<char*>(message),size);
    }
    ASSERT(h);
    handle_ = h;

}

GribHandle::~GribHandle()
{
    grib_handle_delete(handle_);
}


double* GribHandle::getDataValues(size_t& count) const
{
    GRIB_CALL(grib_get_size(handle_, "values", &count));

    double* values = new double[count];
    ASSERT(values);
    size_t n = count;
    GRIB_CALL(grib_get_double_array(handle_,"values",values,&n));
    ASSERT(n == count);
    return values;
}

void GribHandle::setDataValues(const double *values, size_t count)
{
    ASSERT(values);
    GRIB_CALL(grib_set_double_array(handle_,"values",values,count));
}

void GribHandle::write(DataHandle & handle)
{
    const void* message;
    size_t length;
    GRIB_CALL(grib_get_message(handle_, &message, &length));
    ASSERT(handle.write(message, length) == length);
}


GribHandle* GribHandle::clone() const
{
    return new GribHandle(grib_handle_clone(handle_));
}

} // namespace
