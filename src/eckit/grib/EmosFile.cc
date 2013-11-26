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

#include "eckit/io/Buffer.h"
#include "eckit/io/BufferedHandle.h"
#include "eckit/io/MoverHandle.h"
#include "eckit/filesystem/PathName.h"
#include "eckit/config/Resource.h"

#include "EmosFile.h"

#include "eckit/log/Log.h"

using namespace std;
using namespace eckit;

static long readcb(void *data, void *buffer, long len)
{
    DataHandle *handle = reinterpret_cast<eckit::DataHandle*>(data);
    return handle->read(buffer,len);
}

EmosFile::EmosFile(const PathName& path, bool buffered):
    handle_(
        buffered
            ? new BufferedHandle(path.fileHandle(), 64*1024*1024)
            : path.fileHandle()
        )
{
    handle_->openForRead();
}

EmosFile::EmosFile( DataHandle& dh ): 
	handle_(new BufferedHandle( dh, 64*1024*1024)) 
{
    handle_->openForRead();
}

Offset EmosFile::position() 
{
    return handle_->position();
}

void EmosFile::rewind()
{
	handle_->rewind();
}

void EmosFile::seek(const Offset& where)
{
    handle_->seek(where);
}

EmosFile::~EmosFile() 
{
    handle_->close();
}

long EmosFile::read(Buffer& buffer)
{
	size_t len = buffer.size();
	int e    = wmo_read_any_from_stream(handle_.get(),&readcb,buffer,&len);

	if(e == GRIB_SUCCESS)  return len;
	if(e == GRIB_END_OF_FILE) return 0;

	throw ReadError("in EmosFile::read");
}

long EmosFile::readSome(Buffer& buffer)
{
	size_t len = buffer.size();
	int e    = wmo_read_any_from_stream(handle_.get(),&readcb,buffer,&len);

	if(e == GRIB_SUCCESS || e == GRIB_BUFFER_TOO_SMALL)  return len;
	if(e == GRIB_END_OF_FILE)           return 0;

	throw ReadError("in EmosFile::readSome");
}



