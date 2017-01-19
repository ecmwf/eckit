/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File TCPClient.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef eckit_TCPClient_h
#define eckit_TCPClient_h

#include "eckit/net/NetAddress.h"
#include "eckit/net/TCPSocket.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class TCPClient : public TCPSocket {
public:

// -- Contructors

	TCPClient(int port = 0);

// -- Destructor

	~TCPClient();

// -- Methods

	TCPSocket& connect(const std::string& host,int port, int retries = 5, int timeout = 0);

private:

// No copy allowed

	TCPClient(const TCPClient&);
	TCPClient& operator=(const TCPClient&);

// -- Members
	
	int port_;

// -- Overridden methods

	// From TCPSocket

	virtual void bind();
	virtual std::string bindingAddress() const;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif // TCPClient_H
