/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/linalg/Vector.h"

#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/serialisation/Stream.h"

namespace eckit {
namespace linalg {

//----------------------------------------------------------------------------------------------------------------------

Vector::Vector() :
    array_(0), length_(0), own_(false) {}


Vector::Vector(Size length) :
    array_(new Scalar[length]), length_(length), own_(true) {}


Vector::Vector(const Scalar array[], Size length) :
    array_(const_cast<Scalar*>(array)), length_(length), own_(false) {
    ASSERT(array_ && length_ > 0);
}


Vector::Vector(Stream& stream) :
    array_(0), length_(0), own_(false) {
    Size length;
    stream >> length;
    resize(length);

    ASSERT(length_ > 0);
    stream.readBlob(array_, length * sizeof(Scalar));
}


Vector::Vector(const Vector& other) :
    array_(new Scalar[other.length_]), length_(other.length_), own_(true) {
    ::memcpy(array_, other.array_, length_ * sizeof(Scalar));
}


Vector::~Vector() {
    if (own_) {
        delete[] array_;
    }
}


Vector& Vector::operator=(const Vector& other) {
    Vector copy(other);
    swap(copy);
    return *this;
}


void Vector::swap(Vector& other) {
    std::swap(array_, other.array_);
    std::swap(length_, other.length_);
    std::swap(own_, other.own_);
}


void Vector::resize(Size length) {
    Vector v(length);
    swap(v);
}


void Vector::setZero() {
    ::memset(array_, 0, length_ * sizeof(Scalar));
}


void Vector::fill(Scalar value) {
    for (Size i = 0; i < length_; ++i) {
        array_[i] = value;
    }
}


void Vector::encode(Stream& stream) const {
    stream << length_;
    stream.writeBlob(array_, length_ * sizeof(Scalar));
}


Stream& operator<<(Stream& stream, const Vector& vector) {
    vector.encode(stream);
    return stream;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace linalg
}  // namespace eckit
