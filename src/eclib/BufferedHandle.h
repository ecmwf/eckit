/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File BufferedHandle.h
// Manuel Fuentes - ECMWF Jul 96

#ifndef BufferedHandle_H
#define BufferedHandle_H

#include "eclib/Buffer.h"
#include "eclib/DataHandle.h"

class BufferedHandle : public DataHandle {
public:


// -- Contructors

	BufferedHandle(DataHandle*,size_t = 1024*1024);

// -- Destructor

	~BufferedHandle();

// -- Operators


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

	virtual Length estimate();
	virtual Offset position();

    // From Streamable

#if 0
    virtual void encode(Stream&) const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
#endif

// -- Class methods

#if 0
    static  const ClassSpec&  classSpec()        { return classSpec_;}
#endif

private:
    
    void bufferFlush();

// -- Members

	auto_ptr<DataHandle> handle_;
	Buffer               buffer_;
	size_t               pos_;
	size_t               size_;
	size_t               used_;
    bool                 eof_;
    bool                 read_;
    Offset               position_;

    virtual string title() const;

// -- Class members

#if 0
	static  ClassSpec                 classSpec_;
    static  Reanimator<BufferedHandle>  reanimator_;
#endif

};


#endif
