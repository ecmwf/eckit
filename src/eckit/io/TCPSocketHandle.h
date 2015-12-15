/*
 * (C) Copyright 1996-2015 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/TCPSocketHandle.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_filesystem_TCPSocketHandle_h
#define eckit_filesystem_TCPSocketHandle_h

#include "eckit/io/DataHandle.h"
#include "eckit/net/TCPSocket.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class TCPSocketHandle : public DataHandle {
public:

// -- Contructors
	
	TCPSocketHandle(TCPSocket&);

// -- Destructor

	~TCPSocketHandle();

// -- Overridden methods

	// From DataHandle

    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);

	virtual long read(void*,long);
	virtual long write(const void*,long);
	virtual void close();
	virtual void rewind();
	virtual void print(std::ostream&) const;

	// From Streamable


// -- Class methods

protected:

// -- Members

	TCPSocket   connection_;

private:

// No copy allowed

	TCPSocketHandle(const TCPSocketHandle&);
	TCPSocketHandle& operator=(const TCPSocketHandle&);


// -- Class members


};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
