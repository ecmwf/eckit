/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   Vector.h
/// @author Florian Rathgeber
/// @date   June 2015

#ifndef eckit_la_Vector_h
#define eckit_la_Vector_h

#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/la/types.h"

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

// TODO: provide a const view
class Vector {
public:  // methods

    // -- Constructors

    /// Default constructor (empty vector)
    Vector() : v_(0), size_(0), own_(false) {}

    /// Construct vector of given size (allocates memory, not initialised)
    Vector(Size s)
        : v_(new Scalar[s]), size_(s), own_(true) {}

    /// Construct vector from existing data (does NOT take ownership)
    Vector(Scalar* v, Size s)
        : v_(v), size_(s), own_(false) {
        ASSERT(v && size_ > 0);
    }

    /// Copy constructor
    Vector(const Vector& v)
        : v_(new Scalar[v.size_]), size_(v.size_), own_(true) {
        memcpy(v_, v.v_, size_*sizeof(Scalar));
    }

    // TODO: make virtual if used as base class
    ~Vector() {
        if (own_) delete [] v_;
    }

    Vector& operator=(const Vector& v) {
        Vector nv(v);
        swap(nv);
        return *this;
    }

    /// Swap this vector for another
    void swap(Vector& v) {
        std::swap(v_, v.v_);
        std::swap(size_, v.size_);
        std::swap(own_, v.own_);
    }

    /// Resize vector to given size (invalidates data)
    void resize(Size s) {
        Vector v(s);
        swap(v);
    }

    /// Set data to zero
    void setZero() {
        memset(v_, 0, size_*sizeof(Scalar));
    }

    /// @returns size (rows * cols)
    Size size() const { return size_; }
    /// @returns number of rows (i.e. size)
    Size rows() const { return size_; }
    /// @returns number of columns (always 1)
    Size cols() const { return 1; }

    Scalar& operator[](Size i) { return v_[i]; }
    const Scalar& operator[](Size i) const { return v_[i]; }

    /// @returns modifiable view of the data
    Scalar* data() { return v_; }
    /// @returns read-only view of the data
    const Scalar* data() const { return v_; }

protected:  // members
    Scalar* v_;
    Size size_;
    bool own_;
};

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit

#endif
