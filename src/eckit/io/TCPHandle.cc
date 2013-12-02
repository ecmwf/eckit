/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/compat/StrStream.h"
#include "eckit/io/TCPHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

ClassSpec TCPHandle::classSpec_ = {&DataHandle::classSpec(),"TCPHandle",};
Reanimator<TCPHandle> TCPHandle::reanimator_;


void TCPHandle::print(std::ostream& s) const
{
	s << "TCPHandle[host=" << host_ << ",port=" << port_ << ']';
}

void TCPHandle::encode(Stream& s) const
{
	DataHandle::encode(s);
	s << host_;
	s << port_;
}

TCPHandle::TCPHandle(Stream& s):
	DataHandle(s)
{
	s >> host_;
	s >> port_;
}


TCPHandle::TCPHandle(const std::string& host,int port):
	host_(host),
	port_(port)
{
}

TCPHandle::~TCPHandle()
{
}

Length TCPHandle::openForRead()
{
	connection_.connect(host_,port_);
	return 0;
}

void TCPHandle::openForWrite(const Length&)
{
	connection_.connect(host_,port_);
}

void TCPHandle::openForAppend(const Length&)
{
	NOTIMP;
}

long TCPHandle::read(void* buffer,long length)
{
	return connection_.read(buffer,length);
}

long TCPHandle::write(const void* buffer,long length)
{
	return connection_.write(buffer,length);
}

void TCPHandle::close()
{
	connection_.close();
}

void TCPHandle::rewind()
{
	NOTIMP;
}

std::string TCPHandle::title() const
{
	StrStream os;
	os << "TCP[" << host_ << ":" << port_ << "]" << StrStream::ends;
	return std::string(os);
}

//-----------------------------------------------------------------------------

} // namespace eckit

