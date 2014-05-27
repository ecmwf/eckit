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

namespace memory {
namespace detail {

struct SelfManaged
{
    template < typename T >
    static void deallocate( T* p ) { delete p; }
};

struct NotManaged
{
    template < typename T >
    static void deallocate( T* p ) {}
};

class ThreadedLock
{
    Mutex mutex_;
public:
    void lock()   { mutex_.lock(); }
    void unlock() { mutex_.unlock(); }
};

class NoLock
{
public:
    void lock()   {}
    void unlock() {}
};

} // detail
} // memory

//-----------------------------------------------------------------------------

/// Reference counting objects
/// Subclass from this class if you want reference counting object.
/// @note Remember to use 'virtual' inheritance in case of multiple inheritance

template < typename LOCK, typename MEMORY >
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

/// Counted object, with self-managed memory and thread lockable resource
typedef CountedT< memory::detail::ThreadedLock, memory::detail::SelfManaged >  CountedLock;

/// Counted object, with self-managed memory and not thread lockable resource
typedef CountedT< memory::detail::NoLock, memory::detail::SelfManaged >  CountedNoLock;

/// Default Counted type for backward compatibility
/// Same as CountedLock
typedef CountedLock  Counted;

/// Counted object, with third party managed memory and not thread lockable resource
typedef CountedT< memory::detail::ThreadedLock, memory::detail::NotManaged >  OwnedLock;

/// Counted object, with third party managed memory and not thread lockable resource
typedef CountedT< memory::detail::NoLock, memory::detail::NotManaged >  OwnedNoLock;

/// Default Owned type
/// Same as OwnedNoLock
typedef OwnedNoLock Owned;

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
