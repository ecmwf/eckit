/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

// File MemoryPool.h
// Baudouin Raoult - ECMWF May 97

#ifndef eckit_MemoryPool_h
#define eckit_MemoryPool_h

#include "eckit/machine.h"

#include "eckit/memory/NonCopyable.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

struct memblk;

struct MemPool {

	int    pages_;     /* Number of pages to allocate */
	bool   clear_;     /* clear newly allocated memory */
	memblk *first_;

	static MemPool transientPool;
	static MemPool permanentPool;
};

class MemoryPool : private NonCopyable {

public: // class methods


	static void dump(ostream&);
	static void handler();

	static void* largeAllocate(size_t);
	static void  largeDeallocate(void*);

	static void* fastAllocate(size_t,MemPool& = MemPool::transientPool);
	static void  fastDeallocate(void*,MemPool& = MemPool::transientPool);

	static void info(ostream&);

private: // methods

	MemoryPool();
	~MemoryPool();

};

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
