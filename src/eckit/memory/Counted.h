/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Counted.h
/// @date Jun 1996
/// @author Baudouin Raoult
/// @author Tiago Quintino

#ifndef eckit_Counted_h
#define eckit_Counted_h

#include "eckit/memory/NonCopyable.h"
#include "eckit/thread/Mutex.h"
#include "eckit/exception/Exceptions.h"

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
    mutable Mutex mutex_;
public:
    void lock() const    { mutex_.lock(); }
    void unlock()  const { mutex_.unlock(); }
};

class NoLock
{
public:
    void lock() const   {}
    void unlock() const {}
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
    private LOCK {

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
        if ( --count_ == 0 )
        {
            LOCK::unlock();
            MEMORY::deallocate( this );
        }
        else {
            LOCK::unlock();
        }
    }

    void attach() const
    {
        LOCK::lock();
        count_++;
        LOCK::unlock();
    }

    void detach() const
    {
        LOCK::lock();
        try {
            ASSERT(count_ > 1);
            --count_;
        } catch (...) {
            LOCK::unlock();
            throw;
        }
        LOCK::unlock();
    }

    size_t count() const { return count_; }

    void lock() const {
        LOCK::lock();
    }

    void unlock() const {
        LOCK::unlock();
    }

//  void *operator new(size_t s)  { return MemoryPool::fastAllocate(s);}
//  void operator delete(void* p) { MemoryPool::fastDeallocate(p);     }

private: // members

    mutable size_t count_;

};

//-----------------------------------------------------------------------------

/// Counted object, with self-managed memory and thread lockable resource
typedef CountedT< memory::detail::ThreadedLock, memory::detail::SelfManaged >  CountedLock;

/// Counted object, with self-managed memory and not thread lockable resource
typedef CountedT< memory::detail::NoLock, memory::detail::SelfManaged >  CountedNoLock;

/// Default Counted type for backward compatibility
/// Same as CountedLock
typedef CountedLock  Counted;

//-----------------------------------------------------------------------------

} // namespace eckit

#endif
