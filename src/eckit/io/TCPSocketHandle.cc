/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/io/TCPSocketHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

void TCPSocketHandle::print(std::ostream& s) const
{
	s << "TCPSocketHandle[]";
}


TCPSocketHandle::TCPSocketHandle(TCPSocket& s):
    connection_(s)
{
}

TCPSocketHandle::~TCPSocketHandle()
{
}

Length TCPSocketHandle::openForRead()
{
	return 0;
}

void TCPSocketHandle::openForWrite(const Length&)
{
}

void TCPSocketHandle::openForAppend(const Length&)
{
	NOTIMP;
}

long TCPSocketHandle::read(void* buffer,long length)
{
	return connection_.read(buffer,length);
}

long TCPSocketHandle::write(const void* buffer,long length)
{
	return connection_.write(buffer,length);
}

void TCPSocketHandle::close()
{
	connection_.close();
}

void TCPSocketHandle::rewind()
{
	NOTIMP;
}

//-----------------------------------------------------------------------------

} // namespace eckit

