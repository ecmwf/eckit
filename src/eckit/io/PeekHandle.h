/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File io/PeekHandle.h
// Baudouin Raoult - ECMWF May 2020

#ifndef eckit_filesystem_PeekHandle_h
#define eckit_filesystem_PeekHandle_h

#include "eckit/io/Buffer.h"
#include "eckit/io/HandleHolder.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

class PeekHandle : public DataHandle, public HandleHolder {
public:


    /// Contructor, taking ownership

	PeekHandle(DataHandle*);

    /// Contructor, not taking ownership

	PeekHandle(DataHandle&);

    /// Destructor

	virtual ~PeekHandle();

// -- Operators

// -- Methods

    unsigned char peek(size_t);

// -- Overridden methods

    // From DataHandle

    virtual Length openForRead();

    virtual long read(void*,long);
    virtual void close();

    virtual void rewind();
    virtual void print(std::ostream&) const;
    virtual void skip(const Length&);

    virtual Offset seek(const Offset&);
    virtual bool canSeek() const;

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

private:


private: // members

	std::vector<unsigned char> peek_;

    virtual std::string title() const;

// -- Class members

#if 0
	static  ClassSpec                 classSpec_;
    static  Reanimator<PeekHandle>  reanimator_;
#endif

};

//-----------------------------------------------------------------------------

} // namespace eckit


#endif
