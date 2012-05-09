/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/StrStream.h"
#include "eclib/TCPStream.h"

// Server

TCPStream::TCPStream(TCPSocket& socket):
	socket_(socket)
{
}

TCPStream::~TCPStream()
{
}

void TCPStream::closeOutput()
{	
  socket_.closeOutput();
}
//======================
// Tricky solution to be removed when 'mutable' is available
//
string TCPStreamBase::nonConstName()
{
	StrStream r;

	r << "TCP stream from " << socket().localHost() << 
		" at " << socket().localPort();
	r << " to " << socket().remoteHost() << 
		" at " << socket().remotePort() << StrStream::ends;
	return string(r);
}

string TCPStreamBase::name() const
{
	return ((TCPStreamBase*)this)->nonConstName();
}

SharedTCPStream::SharedTCPStream(TCPSocket& s):
	TCPStream(s)
{
}

SharedTCPStream::~SharedTCPStream() 
{ 
}

