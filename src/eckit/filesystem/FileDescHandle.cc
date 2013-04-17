/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/filesystem/FileDescHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

void FileDescHandle::print(ostream& s) const
{
	s << "FileDescHandle[fd=" << fd_ << ']';
}

void FileDescHandle::encode(Stream& s) const
{
	NOTIMP;
}

FileDescHandle::FileDescHandle(int fd):
	fd_(fd)
{
}

FileDescHandle::~FileDescHandle()
{
}

Length FileDescHandle::openForRead()
{
	return 0;
}

void FileDescHandle::openForWrite(const Length&)
{
}

void FileDescHandle::openForAppend(const Length&)
{
}

long FileDescHandle::read(void* buffer,long length)
{
	return ::read(fd_,buffer,length);
}

long FileDescHandle::write(const void* buffer,long length)
{
	return ::write(fd_,buffer,length);
}

void FileDescHandle::close()
{
	// May be we should close fd_ here ?
}

//-----------------------------------------------------------------------------

} // namespace eckit
