/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_memory_SharedPtr_h
#define eckit_memory_SharedPtr_h

#include <cstddef>
#include <utility>

#include "eckit/deprecated.h"

/// @file SharedPtr.h
/// @author Tiago Quintino
/// @date Mar 2014

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// @brief Deletes a pointer and makes sure it is set to 0 afterwards
/// Do not use this function with data allocate with new [].
/// @pre p has been allocated with operator new
/// @param p pointer to be deleted
/// @post  p equals 0
template <class T>
void delete_ptr(T*& p) {
    delete p;
    p = 0;
}

/// @brief Deletes a pointer and makes sure it is set to 0 afterwards
/// Do not use this function with data allocate with new.
/// @pre p has been allocated with operator new []
/// @param p pointer to be deleted
/// @post  p equals CFNULL
template <class T>
void delete_ptr_array(T*& p) {
    delete[] p;
    p = 0;
}

namespace detail {
void assert_valid_SharedPtr(bool);
}

//----------------------------------------------------------------------------------------------------------------------

template <class T>
struct NewDealloc {
    static void deallocate(T*& p) { delete_ptr(p); }
};

template <class T>
struct NewArrayDealloc {
    static void deallocate(T*& p) { delete_ptr_array(p); }
};

//----------------------------------------------------------------------------------------------------------------------

/// A smart pointer that allows to share resources that have derived from Owned
/// @see Owned

template <class T, class ALLOC = NewDealloc<T> >
class DEPRECATED("Use C++11 std::shared_ptr instead") SharedPtr {
public:  // types

    typedef T element_type;
    typedef T* pointer_type;

public:  // methods

    /// Constructor
    SharedPtr() : ptr_(0) {}

    /// Constructor.
    /// @param ptr naked pointer
    explicit SharedPtr(T* ptr) : ptr_(ptr) {
        if (!null()) {
            ptr_->attach();
        }
    }

    /// Copy constructor
    SharedPtr(const SharedPtr& other) : ptr_(other.ptr_) {
        if (!null()) {
            ptr_->attach();
        }
    }

    /// Destructor
    ~SharedPtr() { release(); }

    /// Release ownership
    /// @post ptr_ = 0
    void release() {
        if (!null()) {
            ptr_->lock();
            ptr_->detach(); /* lock/unlock in detach() isn't sufficient, else there is race
                               condition on owners() */

            if (ptr_->owners() == 0) {
                ptr_->unlock();
                ALLOC::deallocate(ptr_);  // also zeros ptr_
                return;
            }
            ptr_->unlock();
            ptr_ = 0;
        }
    }


    /// Reset the ptr
    /// @post ptr_ = other
    void reset(T* other) {
        if (other == ptr_) {
            return;
        }

        release();

        ptr_ = other;

        if (!null()) {
            ptr_->attach();
        }
    }

    /// Reset the ptr
    /// @post ptr_ = other
    void reset(const SharedPtr& other) {
        release();

        ptr_ = other.ptr_;

        if (!null()) {
            ptr_->attach();
        }
    }

    /// Overloading of "=" with SharedPtr
    /// @return missing documentation
    const SharedPtr& operator=(const SharedPtr& other) {
        if (ptr_ != other.ptr_) {
            reset(other);
        }
        return *this;
    }

    /// Overloading of "=" with SharedPtr
    /// @return missing documentation
    const SharedPtr& operator=(T* other) {
        if (ptr_ != other) {
            reset(other);
        }
        return *this;
    }

    /// Swap internal pointer with other SharedPtr
    void swap(SharedPtr& other) { std::swap(ptr_, other.ptr_); }

    /// Overloading of "=="
    /// @return true if pointees have the same address
    operator bool() const { return !null(); }

    /// Overloading of "=="
    /// @return true if pointees have the same address
    bool operator==(const SharedPtr& other) const { return (ptr_ == other.ptr_); }

    /// Overloading of "!="
    /// @return true if pointees have the different address
    bool operator!=(const SharedPtr& other) const { return !operator==(other); }

    /// @returns the naked pointer to the object
    T* get() const { return ptr_; }

    /// @returns true if it is a unique owner
    bool unique() const {
        detail::assert_valid_SharedPtr(null());
        return ptr_->owners() == 1;
    }

    /// Overloading of "->"
    /// @returns the naked pointer to the object
    T* operator->() const {
        detail::assert_valid_SharedPtr(null());
        return ptr_;
    }

    /// Overloading of "*"
    /// @return dereferrences the internal pointer
    T& operator*() const {
        detail::assert_valid_SharedPtr(null());
        return *ptr_;
    }

    size_t owners() const {
        if (!null()) {
            return ptr_->owners();
        }
        return 0;
    }

    size_t use_count() const { return owners(); }

private:  // methods

    /// @returns true if ptr_ == 0
    bool null() const { return (ptr_ == 0); }

private:  // members

    pointer_type ptr_;  ///< raw pointer
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif  // eckit_memory_SharedPtr_h
