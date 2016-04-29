/*
 * (C) Copyright 1996-2016 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File TCPStream.h
// Baudouin Raoult - ECMWF May 96

#ifndef eckit_TCPStream_h
#define eckit_TCPStream_h

#include "eckit/memory/Counted.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/net/TCPSocket.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class TCPServer;

// Choose from 

class TCPStreamBase : public Stream {
public:

// -- Contructors

	TCPStreamBase() {}

// -- Overridden methods

	// From Stream

	virtual long write(const void* buf,long len) 
		{ return socket().write(buf,len); }

	virtual long read(void* buf,long len)
		{ return socket().read(buf,len); }

protected:

// -- Overridden methods

	// From Stream

	virtual std::string name() const;
	

private:

// -- Methods

	std::string nonConstName();
	virtual TCPSocket& socket() = 0;

};

class TCPStream : public TCPStreamBase {
public:

// -- Contructors

	// Take ownership of TCPSocket;

	TCPStream(TCPSocket&); 

// -- Destructor

	~TCPStream();

    // From TCPStreamBase

    virtual TCPSocket& socket() { return socket_; }

protected:

// -- Members

	TCPSocket socket_;

private:

	TCPStream(TCPServer&); 


// -- Overridden methods
	
	// From Stream
	
    virtual void closeOutput();
};

class InstantTCPStream : public TCPStreamBase {
public:

// -- Constructior

	// Does not take ownership of TCPSocket

	InstantTCPStream(TCPSocket& socket):
		socket_(socket) {}

    // -- Overridden methods

    // From TCPStream

    virtual TCPSocket& socket() { return socket_; }

private:

	InstantTCPStream(TCPServer&);

// -- Members

    TCPSocket& socket_;
};

class SharedTCPStream : public TCPStream, public Counted {
public:

// -- Contructors

	SharedTCPStream(TCPSocket&);


private:

// -- Destructor

	~SharedTCPStream();

// -- Contructors
    
	SharedTCPStream(TCPServer&);

// -- Members

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
