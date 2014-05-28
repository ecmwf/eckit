/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @file Counted.h
/// @author Tiago Quintino
/// @date Dec 2013

#include "eckit/exception/Exceptions.h"

#ifndef eckit_SharedPtr_h
#define eckit_SharedPtr_h

//-----------------------------------------------------------------------------

namespace eckit {

//-----------------------------------------------------------------------------

/// @brief Deletes a pointer and makes sure it is set to 0 afterwards
/// Do not use this function with data allocate with new [].
/// @pre p has been allocated with operator new
/// @param p pointer to be deleted
/// @post  p equals 0
template <class T>
void delete_ptr(T*& p)
{
  if( p != 0 )
  {
    delete p;
    p = 0;
  }
}

/// @brief Deletes a pointer and makes sure it is set to 0 afterwards
/// Do not use this function with data allocate with new.
/// @pre p has been allocated with operator new []
/// @param p pointer to be deleted
/// @post  p equals CFNULL
template <class T>
void delete_ptr_array(T*& p)
{
  if( p != 0 )
  {
    delete [] p;
    p = 0;
  }
}

//-----------------------------------------------------------------------------

template <class T>
struct NewDealloc
{
    static void deallocate( T*& p ) { delete_ptr(p); }
};

template <class T>
struct NewArrayDealloc
{
    static void deallocate( T*& p ) { delete_ptr_array(p); }
};

//-----------------------------------------------------------------------------

/// A smart pointer that allows to share resources that have derived from Owned
/// @see Owned

template< class T, class ALLOC = NewDealloc<T> >
class SharedPtr {

public: // types

    typedef T  element_type;
    typedef T* pointer_type;

public: // methods

    /// Constructor
    SharedPtr() : ptr_(0) {}

    /// Constructor.
    /// @param ptr naked pointer
    explicit SharedPtr(T* ptr) : ptr_(ptr)
    {
        if( ! null() )
            ptr_->attach();
    }

    /// Copy constructor
    SharedPtr ( const SharedPtr& other ) : ptr_(other.ptr_)
    {
        if( ! null() )
            ptr_->attach();
    }

    /// Destructor
    ~SharedPtr()
    {
        release();
    }

    /// Release ownership
    /// @post ptr_ = 0
    void release()
    {
        if( ! null() )
        {
            ptr_->detach();

            if( ptr_->owners() == 0 )
                ALLOC::deallocate( ptr_ );

            ptr_ = 0;
        }
    }

    /// Reset the ptr
    /// @post ptr_ = other
    void reset(T* other)
    {
        release();

        ptr_ = other;

        if( ! null() )
            ptr_->attach();
    }

    /// Reset the ptr
    /// @post ptr_ = other
    void reset(const SharedPtr& other)
    {
        release();

        ptr_ = other.ptr_;

        if( ! null() )
            ptr_->attach();
    }

    /// Overloading of "=" with SharedPtr
    /// @return missing documentation
    const SharedPtr& operator= (const SharedPtr& other)
    {
        if( ptr_ != other.ptr_ )
            reset(other);
        return *this;
    }

    /// Overloading of "=" with SharedPtr
    /// @return missing documentation
    const SharedPtr& operator= (T* other)
    {
        if( ptr_ != other )
            reset(other);
        return *this;
    }

    /// Swap internal pointer with other SharedPtr
    void swap ( SharedPtr& other )
    {
        std::swap( ptr_, other.ptr_ );
    }

    /// Overloading of "=="
    /// @return true if pointees have the same address
    operator bool () const
    {
        return ! null();
    }

    /// Overloading of "=="
    /// @return true if pointees have the same address
    bool operator== (const SharedPtr& other) const
    {
        return ( ptr_ == other.ptr_ );
    }

    /// Overloading of "!="
    /// @return true if pointees have the different address
    bool operator!= (const SharedPtr& other) const
    {
        return ! operator==( other );
    }

    /// @returns the naked pointer to the object
    T* get() const { return ptr_; }

    /// @returns true if it is a unique owner
    bool unique() const
    {
        ASSERT( ! null() );
        return ptr_->owners() == 1;
    }

    /// Overloading of "->"
    /// @returns the naked pointer to the object
    T* operator->() const
    {
        ASSERT( ! null() );
        return ptr_;
    }

    /// Overloading of "*"
    /// @return dereferrences the internal pointer
    T& operator*() const
    {
        ASSERT( ! null() );
        return *ptr_;
    }

    size_t owners() const
    {
        if( !null() )
            return ptr_->owners();
        return 0;
    }

    size_t use_count() const { return owners(); }

private: // methods

    /// @returns true if ptr_ == 0
    bool null() const { return ( ptr_ == 0 ); }

private: // members

    pointer_type ptr_; ///< raw pointer

};

//-----------------------------------------------------------------------------

} // namespace eckit

//-----------------------------------------------------------------------------

#endif // eckit_SharedPtr_h
