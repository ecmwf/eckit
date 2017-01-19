/*
 * (C) Copyright 1996-2017 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/BufferedHandle.h
// Manuel Fuentes - ECMWF Jul 96

#ifndef eckit_filesystem_BufferedHandle_h
#define eckit_filesystem_BufferedHandle_h

#include "eckit/io/Buffer.h"
#include "eckit/io/HandleHolder.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class BufferedHandle : public DataHandle, public HandleHolder {
public:


    /// Contructor, taking ownership

	BufferedHandle(DataHandle*,size_t = 1024*1024);

    /// Contructor, not taking ownership

	BufferedHandle(DataHandle&,size_t = 1024*1024);

    /// Destructor

	virtual ~BufferedHandle();

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
    virtual void print(std::ostream&) const;
    virtual void skip(const Length&);

    virtual Offset seek(const Offset&);

	virtual Length estimate();
	virtual Offset position();

    virtual DataHandle* clone() const;

    // From Streamable

#if 0
    virtual void encode(Stream&) const;
    virtual const ReanimatorBase& reanimator() const { return reanimator_; }
#endif

// -- Class methods

#if 0
    static  const ClassSpec&  classSpec()        { return classSpec_;}
#endif

private: // methods
    
    void bufferFlush();

private: // members
    
	Buffer               buffer_;
	size_t               pos_;
	size_t               size_;
	size_t               used_;
    bool                 eof_;
    bool                 read_;
    Offset               position_;
    
    virtual std::string title() const;

// -- Class members

#if 0
	static  ClassSpec                 classSpec_;
    static  Reanimator<BufferedHandle>  reanimator_;
#endif

};

//-----------------------------------------------------------------------------

} // namespace eckit


#endif
