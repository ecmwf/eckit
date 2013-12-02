/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "eckit/config/Resource.h"
#include "eckit/net/TCPClient.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

TCPClient::TCPClient(int port):
	TCPSocket(),
	port_(port)
{
}

TCPClient::~TCPClient()
{
}

void TCPClient::bind()
{
	if(socket_ == -1)
		socket_ = newSocket(port_);
}

std::string TCPClient::bindingAddress() const
{
	//return  Resource<std::string>("localBindingAddr","127.0.0.1");
	return  Resource<std::string>("localBindingAddr","");
}

//-----------------------------------------------------------------------------

} // namespace eckit

