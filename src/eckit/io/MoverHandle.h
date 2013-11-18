/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File filesystem/MoverHandle.h
// Baudouin Raoult - (c) ECMWF Jun 11

#ifndef eckit_filesystem_MoverHandle_h
#define eckit_filesystem_MoverHandle_h

#include "eckit/io/DataHandle.h"
#include "eckit/thread/Mutex.h"
#include "eckit/net/TCPSocket.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class MoverHandle : public DataHandle {
public:

// -- Exceptions
	// None

// -- Contructors

	MoverHandle(DataHandle*);

// -- Destructor

	virtual ~MoverHandle(); 

// -- Convertors
	// None

// -- Operators
	// None

// -- Methods

	DataHandle& handle() { return *handle_; }
	void fail(const string&);

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

protected:

// -- Members
	// None

// -- Methods
	

// -- Overridden methods
	// None

// -- Class members
	// None

// -- Class methods
	// None

private:


// -- Members

    auto_ptr<DataHandle> handle_;
    TCPSocket data_;
    bool fail_;
    string error_;
    Mutex mutex_;

// -- Methods
	// None

// -- Overridden methods
	// None

    virtual Length openForRead() ;
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);
    virtual long read(void*,long); 
    virtual long write(const void*,long);
    virtual void close();
    virtual string title() const;

	void print(ostream&) const; 	

// -- Class members
	// None

// -- Class methods
	// None

// -- Friends

	//friend ostream& operator<<(ostream& s,const MoverHandle& p)
	//	{ p.print(s); return s; }

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
