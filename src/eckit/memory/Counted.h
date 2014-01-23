/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Counted.h
/// @author Baudouin Raoult
/// @date Jun 1996

#ifndef eckit_Counted_h
#define eckit_Counted_h

#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"

#include "eckit/log/Log.h"

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

namespace detail {

struct DoDelete
{
    template < typename T >
    static void deallocate( T* p ) { delete p; }
};

struct DontDelete
{
    template < typename T >
    static void deallocate( T* p ) {}
};

class WithThreadLock
{
    Mutex mutex_;
public:
    void lock()   { mutex_.lock(); }
    void unlock() { mutex_.unlock(); }
};

class NoThreadLock
{
public:
    void lock()   {}
    void unlock() {}
};

} // detail

//-----------------------------------------------------------------------------

/// Reference counting objects
/// Subclass from this class if you want reference counting object.
/// @note Remember to use 'virtual' inheritance in case of multiple inheritance
template < typename LOCK = detail::WithThreadLock,
           typename DEL  = detail::DoDelete >
class CountedT :
        private NonCopyable,
        public LOCK {

public: // methods

    CountedT() : count_(0) {}

    virtual ~CountedT() {}

    void attach()
    {
        LOCK::lock();
        count_++;
        LOCK::unlock();
    }

    void detach()
    {
        LOCK::lock();
        if( --count_ == 0 )
        {
            LOCK::unlock();
            DEL::deallocate( this );
        }
        else
            LOCK::unlock();
    }

    unsigned long count() const { return count_; }

//	void *operator new(size_t s)  { return MemoryPool::fastAllocate(s);}
//	void operator delete(void* p) { MemoryPool::fastDeallocate(p);     } 

private: // members

	unsigned long count_;

};

//-----------------------------------------------------------------------------

typedef CountedT<>  Counted;

typedef CountedT< detail::WithThreadLock, detail::DoDelete >  CountedLock; //< same as Counted
typedef CountedT< detail::NoThreadLock,   detail::DoDelete >  CountedNoLock;

typedef CountedT< detail::WithThreadLock, detail::DontDelete >  CountedSharedLock;
typedef CountedT< detail::NoThreadLock,   detail::DontDelete >  CountedSharedNoLock;

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
