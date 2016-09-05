/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/linalg/Vector.h"

#include <cstring>
#include "eckit/io/Buffer.h"
#include "eckit/serialisation/Stream.h"

namespace eckit {
namespace linalg {

//-----------------------------------------------------------------------------

Vector::Vector() :
    v_(0),
    size_(0),
    own_(false) {
}

//-----------------------------------------------------------------------------

Vector::Vector(Size s) :
    v_(new Scalar[s]),
    size_(s),
    own_(true) {
}

//-----------------------------------------------------------------------------

Vector::Vector(Scalar* v, Size s) :
    v_(v),
    size_(s),
    own_(false) {
    ASSERT(v && size_ > 0);
}

//-----------------------------------------------------------------------------

Vector::Vector(Stream& stream) :
    v_(0),
    size_(0),
    own_(false) {
    Size size;
    resize(size);
    Buffer b(v_, size*sizeof(Scalar), /* dummy */ true);

    stream >> size;
    stream >> b;
}

//-----------------------------------------------------------------------------

Vector::Vector(const Vector& other) :
    v_(new Scalar[other.size_]),
    size_(other.size_),
    own_(true) {
    ::memcpy(v_, other.v_, size_ * sizeof(Scalar));
}

//-----------------------------------------------------------------------------

Vector::~Vector() {
    if (own_) {
        delete [] v_;
    }
}

//-----------------------------------------------------------------------------

Vector&Vector::operator=(const Vector& other) {
    Vector v(other);
    swap(v);
    return *this;
}

//-----------------------------------------------------------------------------

void Vector::swap(Vector& other) {
    std::swap(v_,    other.v_);
    std::swap(size_, other.size_);
    std::swap(own_,  other.own_);
}

//-----------------------------------------------------------------------------

void Vector::resize(Size s) {
    Vector v(s);
    swap(v);
}

//-----------------------------------------------------------------------------

void Vector::setZero() {
    ::memset(v_, 0, size_*sizeof(Scalar));
}

//-----------------------------------------------------------------------------

void Vector::fill(Scalar value) {
    for (Size i = 0; i < size_; ++i) {
        v_[i] = value;
    }
}

//-----------------------------------------------------------------------------

void Vector::encode(Stream& stream) const {
  Buffer b(v_, size_*sizeof(Scalar), /* dummy */ true);

  stream << size_;
  stream << b;
}

//-----------------------------------------------------------------------------

Stream& operator<<(Stream& stream, const Vector& vector) {
    vector.encode(stream);
    return stream;
}

//-----------------------------------------------------------------------------

}  // namespace linalg
} // namespace eckit
