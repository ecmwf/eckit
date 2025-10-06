/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#ifndef eckit_ScopedPtr_h
#define eckit_ScopedPtr_h

#include "eckit/deprecated.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/memory/NonCopyable.h"

namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

/// A smart pointer that deletes the pointee object when going out of scope.
/// This should have a very similar interface to boost scoped_ptr or std unique_ptr
/// so that once C++11 is suported overall compilers we can switch easily.
/// However due to lack of C++11 support, it does not support move semantics.

template <typename T>
class DEPRECATED("Use C++11 std::unique_ptr instead") ScopedPtr : private NonCopyable {

public:  // types

    typedef T element_type;
    typedef T* pointer_type;
    typedef T& reference_type;

public:  // methods

    /// Constructor
    /// @throws nothing
    explicit ScopedPtr(pointer_type ptr = 0) : ptr_(ptr) {}

    /// Destructor
    /// @throws nothing
    ~ScopedPtr() { destroy(); }

    /// Resets the pointee
    /// @throws nothing
    void reset(pointer_type ptr = 0) {
        destroy();
        ptr_ = ptr;
    }

    /// Releases the ownership of the pointee
    /// @throws nothing
    pointer_type release() {
        pointer_type r = ptr_;
        ptr_           = 0;
        return r;
    }

    /// Assignement operator transfers ownership to another ScopedPtr
    const ScopedPtr& operator=(ScopedPtr& other) {
        reset(other.release());
        return *this;
    }

    /// Dereferences the pointee
    reference_type operator*() const {
        ASSERT(ptr_);
        return *ptr_;
    }

    /// Dereferences object member
    pointer_type operator->() const {
        ASSERT(ptr_);
        return ptr_;
    }

    /// @returns a pointer to the managed object or null if no object is owned.
    /// Should be used with caution, because of issues dealing with raw pointers.
    /// However, get makes it possible to explicitly test whether the stored point is NULL.
    /// The function never throws. get is typically used when calling functions
    /// that require a raw pointer.
    /// Note: previously this asserted ptr_ was not null, however this is inconsistent
    ///       with the standard boost scoped_ptr or std unique_ptr
    pointer_type get() const { return ptr_; }

    /// @returns true if pointer is not null
    /// @throws nothing
    operator bool() const { return (ptr_ != 0); }

    /// Swaps the pointee with another ScopedPtr
    /// @throws nothing
    void swap(ScopedPtr<T>& other) {
        pointer_type tmp(ptr_);
        ptr_       = other.ptr_;
        other.ptr_ = tmp;
    }

protected:  // methods

    void destroy() {
        delete ptr_;
        ptr_ = 0;
    }

private:  // members

    pointer_type ptr_;
};

/// non-member function overload
template <typename T>
void swap(ScopedPtr<T>& a, ScopedPtr<T>& b) {
    a.swap(b);
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

#endif
