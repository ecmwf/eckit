/*
 * (C) Copyright 1996-2013 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_ScopedPtr_h
#define eckit_ScopedPtr_h

#include "eckit/memory/NonCopyable.h"
#include "eckit/exception/Exceptions.h"


//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

template < typename T >
class ScopedPtr : private NonCopyable {

public: // types

    typedef T element_type;
    
public: // methods

    /// Constructor
    /// @throws nothing
    explicit ScopedPtr( T* ptr = 0 ) : ptr_(ptr) {}
    
    /// Destructor
    /// @throws nothing
    ~ScopedPtr() { release(); }
    
    /// Resets the pointee
    /// @throws nothing
    void reset( T* ptr = 0 )
    {
        release();
        ptr_ = ptr;
    }
    
    /// Dereferences the pointee
    T& operator*() const { ASSERT(ptr_); return *ptr_; }

    /// Calling operator
    T* operator->() const { ASSERT(ptr_); return ptr_; }
    
    /// @returns the stored pounter
    /// Should be used with caution, because of issues dealing with raw pointers.
    /// However, get makes it possible to explicitly test whether the stored point is NULL.
    /// The function never throws. get is typically used when calling functions
    /// that require a raw pointer.
    /// Note: previously this asserted ptr_ was not null, however this is in-consistent
    ///       with the standard std/boost::scoped_ptr.
    T* get() const { return ptr_; }
    
    /// @returns true if pointer is not null
    /// @throws nothing
    operator bool() const { return (ptr_ != 0); }
    
    /// Swaps the pointee with another ScopedPtr
    /// @throws nothing
    void swap( ScopedPtr<T>& other ) 
    {
        T* tmp( ptr_ ); 
        ptr_ = other.ptr_; 
        other.ptr_ = tmp;
    }
    
protected: // methods
    
    void release() { if(ptr_) delete ptr_; ptr_ = 0; }
    
private: // members 

    T* ptr_;
    
};

template< typename T > 
void swap( ScopedPtr<T>& a, ScopedPtr<T>& b )
{
    a.swap(b);
}


//-----------------------------------------------------------------------------

} // namespace eckit

#endif
