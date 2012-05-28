/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File Streamable.h
// B.Raoult - ECMWF May-1996

#ifndef Streamable_H
#define Streamable_H

#include "eclib/MemoryPool.h"
#include "eclib/Reanimator.h"
#include "eclib/Stream.h"

class Streamable {
public:

	friend Stream& operator<<(Stream&,const Streamable&);

// -- Contructors

	Streamable() 			{}
	Streamable(Stream&);

// -- Destructor

	virtual ~Streamable()   {}

// -- Methods
	
	virtual string className() const;
	virtual const ReanimatorBase& reanimator() const { return reanimator_; }

// -- Class methods

    static  const ClassSpec&  classSpec()        { return classSpec_; }

// -- Operators

	void *operator new(size_t s)          { return MemoryPool::fastAllocate(s);}
	void *operator new(size_t s,void *p)  { return p;                          }
	void operator delete(void* p)         { MemoryPool::fastDeallocate(p);     } 

protected:

// -- Methods
	
	virtual void encode(Stream&) const;

	bool sameClass(const Streamable&) const;

private:

// No copy allowed

	Streamable(const Streamable&);
	Streamable& operator=(const Streamable&);

// -- Class members
	
	static  ClassSpec classSpec_;
	static  Reanimator<Streamable> reanimator_;

};

#endif
