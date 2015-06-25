/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <cstring>

#include "eckit/la/Vector.h"
#include "eckit/serialisation/Stream.h"

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

Vector::Vector() : v_(0), size_(0), own_(false) {}

//-----------------------------------------------------------------------------

Vector::Vector(Size s) : v_(new Scalar[s]), size_(s), own_(true) {}

//-----------------------------------------------------------------------------

Vector::Vector(Scalar* v, Size s) : v_(v), size_(s), own_(false) {
    ASSERT(v && size_ > 0);
}

//-----------------------------------------------------------------------------

Vector::Vector(Stream& s) : v_(0), size_(0), own_(false) {
    Size size;
    s >> size;
    resize(size);
    for (Size i = 0; i < size; ++i) s >> v_[i];
}

//-----------------------------------------------------------------------------

Vector::Vector(const Vector& v)
  : v_(new Scalar[v.size_]), size_(v.size_), own_(true) {
    ::memcpy(v_, v.v_, size_ * sizeof(Scalar));
}

//-----------------------------------------------------------------------------

Vector::~Vector() {
    if (own_) delete [] v_;
}

//-----------------------------------------------------------------------------

Vector&Vector::operator=(const Vector& v) {
    Vector nv(v);
    swap(nv);
    return *this;
}

//-----------------------------------------------------------------------------

void Vector::swap(Vector& v) {
    std::swap(v_, v.v_);
    std::swap(size_, v.size_);
    std::swap(own_, v.own_);
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

void Vector::fill(Scalar s) {
    for (Size i = 0; i < size_; ++i) v_[i] = s;
}

//-----------------------------------------------------------------------------

void Vector::encode(Stream& s) const {
  s << size_;
  for (Size i = 0; i < size_; ++i) s << v_[i];
}

//-----------------------------------------------------------------------------

Stream& operator<<(Stream& s, const Vector& v) {
    v.encode(s);
    return s;
}

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit
