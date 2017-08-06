/*
 * (C) Copyright 1996-2017 ECMWF.
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

#include "eckit/eckit.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/utils/Hash.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

// NOTE: I am not realy happy about those classes, so they may change...
//       Note that this class calls:
//          signal(SIGPIPE,SIG_IGN);

class TCPSocket  {
public:

// -- Execptions

    class UnknownHost : public Exception { public: UnknownHost(const std::string&); };

// -- Contructors

	TCPSocket();

	// From an existing TCPSocket (see TCPServer::accept)

    /// @warning
    /// **** NOTE: copying gives ownership of the socket to new object
    /// **** Beware of 'slicing', i.e copying subclasses.

	TCPSocket(TCPSocket&);

// -- Destructor

	virtual ~TCPSocket();

// -- Operators

	// See note on copy constructor

	TCPSocket& operator=(TCPSocket&);

// -- Methods

	// I/O
    long write(const void* buf, long length);

	/// Read from a TCP socket
	///
	/// \param buf The buffer to read into
	/// \param length The maximum number of bytes to read
	///
	/// **Configuration flags**
	/// \arg **useSelectOnTCPSocket** (*bool*): use select for improved resilience
	///   on flaky connections
	/// \arg **socketSelectTimeout** (*long*): timeout in seconds for the select
	///   (only if **useSelectOnTCPSocket** is enabled)
	long read(void * buf,long length);

	long rawRead(void*,long); // Non-blocking version

	bool isConnected() const { return socket_ != -1; }
	bool stillConnected() const;

	// close sockets
	virtual void close();

	// peer name

	in_addr       remoteAddr() const;
    const std::string& remoteHost() const;
	int           remotePort() const;

	in_addr       localAddr() const;
    const std::string& localHost() const;
	int           localPort() const;

	void          bufferSize(int n) { bufSize_ = n; }
	virtual int  socket();

	void closeOutput();
    void closeInput();

    void debug(bool on);

// -- Class methods

    static std::string  addrToHost(in_addr);
    static in_addr hostToAddr(const std::string&);
    static std::string hostName(const std::string& h, bool full = false);

protected:

// -- Members

	int      socket_;      // i/o socket
	int      localPort_;   // effective port
	int      remotePort_;  // remote port
    std::string   remoteHost_;  // remote host
	in_addr  remoteAddr_;  // remote ip adress
    std::string   localHost_;   // local host
	in_addr  localAddr_;   // local ip adress
	int      bufSize_;

    // Debug
    bool     debug_;
    bool     newline_;
    char     mode_;

// -- Methods

	int newSocket(int);



private:

// -- Methods

	virtual void bind();  // The socket must be made
    virtual std::string bindingAddress() const;

// -- Friends


};

std::ostream& operator<<(std::ostream&,in_addr);


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
