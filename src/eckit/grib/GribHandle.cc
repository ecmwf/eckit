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
#include "eckit/utils/StringTools.h"
#include "GribHandle.h"

using namespace eckit;

GribHandle::GribHandle(const Buffer& buffer,size_t size)
    :handle_(0)
{
    const char *message = buffer;
    grib_handle *h = grib_handle_new_from_message_copy(0,const_cast<char*>(message),size);
    ASSERT(h);
    handle_ = h;
}

GribHandle::~GribHandle()
{
    grib_handle_delete(handle_);
}
