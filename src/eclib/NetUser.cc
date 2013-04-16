/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eclib/NetUser.h"
#include "eckit/io/SockBuf.h"
#include "eclib/TCPStream.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

NetUser::NetUser(TCPSocket& protocol):
	protocol_(protocol)
{
	//Log::info() << "New service connection from " << protocol_.remoteHost() << endl;
}

NetUser::~NetUser()
{
	//Log::info() << "End of service connection from " << protocol_.remoteHost() << endl;
}			

void NetUser::run()
{
	SockBuf         buf(protocol_);
	ostream         out(&buf);
	istream         in(&buf);
	InstantTCPStream  stream(protocol_);

	serve(stream,in,out);
}

//-----------------------------------------------------------------------------

} // namespace eckit

