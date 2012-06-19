/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File TCPStream.h
// Baudouin Raoult - ECMWF May 96

#ifndef TCPStream_H
#define TCPStream_H

#include "eclib/Counted.h"
#include "eclib/NonCopyable.h"
#include "eclib/Stream.h"
#include "eclib/TCPSocket.h"

class TCPServer;

// Choose from 

class TCPStreamBase : public Stream,
                      private NonCopyable {
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

	virtual string name() const;
	

private:

// -- Methods

	string nonConstName();
	virtual TCPSocket& socket() = 0;

};

class TCPStream : public TCPStreamBase,
                  private NonCopyable {
public:

// -- Contructors

	// Take ownership of TCPSocket;

	TCPStream(TCPSocket&); 

// -- Destructor

	~TCPStream();

protected:

// -- Members

	TCPSocket socket_;

private:

	TCPStream(TCPServer&); 


// -- Overridden methods
	
	// From Stream
	
	virtual void closeOutput();

	// From TCPStream

	virtual TCPSocket& socket() { return socket_; }
};

class InstantTCPStream : public TCPStreamBase {
public:

// -- Constructior

	// Does not take ownership of TCPSocket

	InstantTCPStream(TCPSocket& socket):
		socket_(socket) {}

private:

// No copy allowed

	InstantTCPStream(const InstantTCPStream&);
	InstantTCPStream& operator=(const InstantTCPStream&);

	InstantTCPStream(TCPServer&);

// -- Members

	TCPSocket& socket_;

// -- Overridden methods

	// From TCPStream

	virtual TCPSocket& socket() { return socket_; }
};

class SharedTCPStream : public TCPStream, public Counted {
public:

// -- Contructors

	SharedTCPStream(TCPSocket&);


private:

// -- Destructor

	~SharedTCPStream();

// No copy allowed

	SharedTCPStream(const SharedTCPStream&);
	SharedTCPStream& operator=(const SharedTCPStream&);

	SharedTCPStream(TCPServer&);

// -- Members

};

#endif
