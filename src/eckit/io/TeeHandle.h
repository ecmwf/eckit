/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/TeeHandle.h
// Manuel Fuentes - ECMWF Jul 96

#ifndef eckit_filesystem_TeeHandle_h
#define eckit_filesystem_TeeHandle_h

#include "eckit/io/DataHandle.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class TeeHandle : public DataHandle {
public:

	typedef vector<DataHandle*> HandleList;

// -- Contructors

	TeeHandle();
	TeeHandle(DataHandle*, DataHandle*);
	TeeHandle(const HandleList&);
	TeeHandle(Stream&);

// -- Destructor

	~TeeHandle();

// -- Operators

	virtual void operator+=(DataHandle*);

// -- Overridden methods

    // From DataHandle

    virtual Length openForRead();
    virtual void openForWrite(const Length&);
    virtual void openForAppend(const Length&);

    virtual long read(void*,long);
    virtual long write(const void*,long);
    virtual void close();
    virtual void flush();
    virtual void rewind();
    virtual void print(ostream&) const;


    virtual void toRemote(Stream&) const;
    virtual void toLocal(Stream&) const;
    virtual DataHandle* toLocal();
    virtual void cost(map<string,Length>&, bool) const;

    // From Streamable

    virtual void encode(Stream&) const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

    static  const ClassSpec&  classSpec()        { return classSpec_;}

private:

// -- Members

	HandleList			    datahandles_;

// -- Methods

// -- Class members

	static  ClassSpec                 classSpec_;
    static  Reanimator<TeeHandle>  reanimator_;

};



//-----------------------------------------------------------------------------

} // namespace eckit

#endif
