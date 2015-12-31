/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File NetService.h
// Baudouin Raoult - ECMWF Jun 96

#ifndef eckit_NetService_h
#define eckit_NetService_h

#include "eckit/net/TCPServer.h"
#include "eckit/thread/Thread.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class NetUser;

class NetService : public Thread {

public:

// -- Contructors

    /// @param[in]  port     TCP port to listen on
    /// @param[in]  visible  Make the thread this service is running in visible
    ///                      on the Monitor (defaults to false)
    NetService(int port, bool visible = false);

// -- Destructor

	~NetService();

// -- Methods

    /// @returns hostname to which this server is answering
    std::string hostname() const;
    /// @returns port to which this server is answering
    int port() const;

// -- Overriden methods

	// From Task

	virtual void run();

private:

// -- Members

	TCPServer server_;

    bool visible_; ///< Visible on the Monitor?

// -- Methods

	virtual NetUser* newUser(TCPSocket&) = 0;	
	virtual std::string   name() = 0;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif

