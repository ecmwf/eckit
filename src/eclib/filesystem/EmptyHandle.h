/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File filesystem/EmptyHandle.h
// Manuel Fuentes - ECMWF Jul 96

#ifndef eckit_filesystem_EmptyHandle_h
#define eckit_filesystem_EmptyHandle_h

#include "eclib/DataHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class EmptyHandle :  public DataHandle {
public:

// -- Contructors
	EmptyHandle()							{ }
	EmptyHandle(Stream& s): DataHandle(s) 	{ }

// -- Destructor

	~EmptyHandle() 							{ }

// -- Overridden methods

    // From DataHandle

    virtual Length openForRead()              { return 0;}
    virtual void openForWrite(const Length&)  {}
    virtual void openForAppend(const Length&) {}

    virtual long   read(void*,long)			{ return 0; }
    virtual long   write(const void*,long n){ return n; }
    virtual void   close()					{ }
    virtual void   flush()					{ }
    virtual void   rewind()					{ }
    virtual void   print(ostream& s) const	{ s << "Empty Handle"; }
	virtual bool   isEmpty() const          { return true; }

    // From Streamable

    virtual void encode(Stream& s) const		 { DataHandle::encode(s); }
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

    static  const ClassSpec&  classSpec()        { return classSpec_;}

private:

// -- Class members

    static  ClassSpec                 classSpec_;
    static  Reanimator<EmptyHandle>  reanimator_;

};


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
