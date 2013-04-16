/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_TCPSocket_h
#define eckit_TCPSocket_h

#include <netinet/in.h>

#include "eclib/machine.h"
#include "eckit/exception/Exceptions.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// NOTE: I am not realy happy about those classes, so they may change...
//       Note that this class calls:
//          signal(SIGPIPE,SIG_IGN);

class TCPSocket  {
public:

// -- Execptions

	class UnknownHost : public Exception { public: UnknownHost(const string&); };

// -- Contructors

	TCPSocket();

	// From an existing TCPSocket (see TCPServer::accept)

	// **** NOTE: copying give ownership of the socket to new object
	// **** Beware of 'slicing', i.e copying subclasses.

	TCPSocket(TCPSocket&);

// -- Destructor

	virtual ~TCPSocket();

// -- Operators

	// See note on copy constructor

	TCPSocket& operator=(TCPSocket&);

// -- Methods

	// I/O
	long write(const void*,long);
	long read(void*,long);

	long rawRead(void*,long); // Non-blocking version

	bool isConnected() const { return socket_ != -1; }
	bool stillConnected() const;

	// close sockets
	virtual void close();

	// peer name

	in_addr       remoteAddr() const;
	const string& remoteHost() const;
	int           remotePort() const;

	in_addr       localAddr() const;
	const string& localHost() const;
	int           localPort() const;

	void          bufferSize(int n) { bufSize_ = n; }
	virtual int  socket();
	
	void closeOutput();
    void closeInput();

// -- Class methods 

	static string  addrToHost(in_addr);
	static in_addr hostToAddr(const string&);
	static string hostName(const string& h, bool full = false);

protected:

// -- Members

	int      socket_;      // i/o socket
	int      localPort_;   // effective port
	int      remotePort_;  // remote port
	string   remoteHost_;  // remote host
	in_addr  remoteAddr_;  // remote ip adress
	string   localHost_;   // local host
	in_addr  localAddr_;   // local ip adress
	int      bufSize_;

// -- Methods

	int newSocket(int);



private:

// -- Methods

	virtual void bind();  // The socket must be made
	virtual string bindingAddress() const;

// -- Friends


};

ostream& operator<<(ostream&,in_addr);


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
