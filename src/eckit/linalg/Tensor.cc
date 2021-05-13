/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/linalg/Tensor.h"

#include <cstring>

#include "eckit/io/Buffer.h"
#include "eckit/serialisation/Stream.h"

namespace eckit {
namespace linalg {

//----------------------------------------------------------------------------------------------------------------------

Tensor::Tensor() : array_(0), size_(0), shape_(0), own_(false) {}

Tensor::Tensor(const std::vector<Size>& shape) : array_(nullptr), shape_(shape), own_(true) {
    size_ = flatten(shape_);
    ASSERT(size() > 0);
    array_ = new Scalar[size_];
    ASSERT(array_);
}


Tensor::Tensor(const Scalar* array, const std::vector<Size>& shape) :
    array_(const_cast<Scalar*>(array)), own_(false) {
    shape_ = shape;
    size_  = flatten(shape);
    ASSERT(size() > 0);
    ASSERT(array_);
}


Tensor::Tensor(Stream& stream) : array_(0), size_(0), shape_(0), own_(true) {
    Size shape_size;
    stream >> shape_size;
    shape_.resize(shape_size);
    for(auto& v: shape_)
        stream >> v;
    resize(shape_);
    ASSERT(size() > 0);
    ASSERT(array_);
    stream.readBlob(array_, size() * sizeof(Scalar) );
}


void Tensor::encode(Stream& stream) const {
    stream << shape_.size();
    for (auto v : shape_)
        stream << v;
    stream.writeBlob(const_cast<Scalar*>(array_), size() * sizeof(Scalar));
}


Tensor::Tensor(const Tensor& other) :
    array_(new Scalar[other.size()]), size_(other.size_), shape_(other.shape_), own_(true) {
    ASSERT(size() > 0);
    ASSERT(array_);
    ::memcpy(array_, other.array_, size() * sizeof(Scalar));
}


Tensor::~Tensor() {
    if (own_) {
        delete[] array_;
    }
}


Tensor& Tensor::operator=(const Tensor& other) {
    // do not optimize for if size() == other.size(), as using copy constructor
    // consistently retains ownership (no surprises in ownership behaviour)
    Tensor copy(other);
    swap(copy);
    return *this;
}


void Tensor::swap(Tensor& other) {
    std::swap(array_, other.array_);
    std::swap(size_, other.size_);
    std::swap(shape_, other.shape_);
    std::swap(own_, other.own_);
}


void Tensor::resize(const std::vector<Size>& shape) {
    // avoid reallocation if memory is the same size
    Size newsize = flatten(shape);
    if (this->size() != newsize) {
        Tensor m(shape);
        swap(m);
    }
}


void Tensor::zero() {
    ASSERT(size() > 0);
    ASSERT(array_);
    ::memset(array_, 0, size() * sizeof(Scalar));
}


void Tensor::fill(Scalar value) {
    for (Size i = 0; i < size(); ++i) {
        array_[i] = value;
    }
}

Stream& operator<<(Stream& stream, const Tensor& t) {
    t.encode(stream);
    return stream;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace linalg
}  // namespace eckit
