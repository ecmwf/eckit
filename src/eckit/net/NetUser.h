/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_NetUser_h
#define eckit_NetUser_h

#include "eckit/net/TCPSocket.h"
#include "eckit/thread/Thread.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class Stream;

class NetUser : public Thread {
public:


// -- Contructors

	NetUser(TCPSocket&);

// -- Destructor

	~NetUser();


protected:

// -- Members

	TCPSocket protocol_;

private:

// -- Methods

    virtual void serve(Stream&, std::istream&, std::ostream&) = 0;

// -- Overridden methods
	// From Task

	virtual void run();

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
