/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/TCPHandle.h
// Baudouin Raoult - ECMWF Jul 96

#ifndef eckit_filesystem_TCPHandle_h
#define eckit_filesystem_TCPHandle_h

#include "eckit/io/DataHandle.h"
#include "eckit/net/TCPClient.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class TCPHandle : public DataHandle {
public:

// -- Contructors
	
	TCPHandle(Stream&);
	TCPHandle(const std::string& host,int port);

// -- Destructor

	~TCPHandle();

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
	virtual std::string title() const;
    virtual bool moveable() const { return true; }

	// From Streamable

	virtual void encode(Stream&) const;
	virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

	static  const ClassSpec&  classSpec()         { return classSpec_;}

protected:

// -- Members

	std::string      host_;
	int         port_;
	TCPClient   connection_;

private:


// -- Class members

    static  ClassSpec               classSpec_;
	static  Reanimator<TCPHandle>  reanimator_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
