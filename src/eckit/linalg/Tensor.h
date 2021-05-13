/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   Tensor.h
/// @author Tiago Quintino
/// @date   May 2021

#pragma once

#include <cstring>
#include <functional>
#include <numeric>
#include <vector>

#include "eckit/io/Buffer.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/linalg/types.h"
#include "eckit/exception/Exceptions.h"

namespace eckit {
namespace linalg {

//----------------------------------------------------------------------------------------------------------------------

/// Dense Tensor in column major storage order
template<typename S>
class Tensor {
public:  // types
    typedef eckit::linalg::Size Size;

public: // class methods
    
    static Size flatten(const std::vector<Size>& shape) {
        return std::accumulate(std::begin(shape), std::end(shape), 1, std::multiplies<Size>());
    }

public:  // methods
    
    /// Default constructor (empty tensor)
    Tensor() : array_(0), size_(0), shape_(0), own_(false) {}

    /// Construct tensor with given rows and columns (allocates memory, not initialised)
    Tensor(const std::vector<Size>& shape) : array_(nullptr), shape_(shape), own_(true) {
        size_ = flatten(shape_);
        ASSERT(size() > 0);
        array_ = new S[size_];
        ASSERT(array_);
    }

    /// Construct tensor from existing data (does NOT take ownership)
    Tensor(const S* array, const std::vector<Size>& shape) : array_(const_cast<S*>(array)), own_(false) {
        shape_ = shape;
        size_  = flatten(shape_);
        ASSERT(size() > 0);
        ASSERT(array_);
    }

    /// Constructor from Stream
    Tensor(Stream& s) : array_(0), size_(0), shape_(0), own_(true) {
        Size shape_size;
        s >> shape_size;
        shape_.resize(shape_size);
        for (auto& v : shape_)
            s >> v;
        resize(shape_);
        ASSERT(size() > 0);
        ASSERT(array_);
        s.readBlob(array_, size() * sizeof(S));
    }

    /// Copy constructor
    Tensor(const Tensor& other) : array_(new S[other.size()]), size_(other.size_), shape_(other.shape_), own_(true) {
        ASSERT(size() > 0);
        ASSERT(array_);
        ::memcpy(array_, other.array_, size() * sizeof(S));
    }

    /// Destructor
    ~Tensor() {
        if (own_) {
            delete[] array_;
        }
    }

    /// Assignment
    /// Not optimized for if size() == other.size(), as using copy constructor
    /// consistently retains ownership to avoid surprises in ownership behaviour.
    Tensor& operator=(const Tensor& other) {
        Tensor copy(other);
        swap(copy);
        return *this;
    }

    /// Swap this tensor with another
    void swap(Tensor& other) {
        std::swap(array_, other.array_);
        std::swap(size_, other.size_);
        std::swap(shape_, other.shape_);
        std::swap(own_, other.own_);
    }

    /// Resize tensor to given a shape
    /// Invalidates data if shapes don't match, otherwise keeps data and simply reshapes
    void resize(const std::vector<Size>& shape) {
        if (this->size() != flatten(shape)) {  // avoid reallocation if same size
            Tensor m(shape);
            swap(m);
        }
        else {
            shape_ = shape;
        }
    }


    /// Set data to zero
    void zero() {
        ASSERT(size() > 0);
        ASSERT(array_);
        ::memset(array_, 0, size() * sizeof(S));
    }

    /// Fill vector with given S
    void fill(S value) {
        for (Size i = 0; i < size(); ++i) {
            array_[i] = value;
        }
    }

    /// Serialise to a Stream
    /// This serialisation is not cross-platform
    void encode(Stream& s) const {
        s << shape_.size();
        for (auto v : shape_)
            s << v;
        s.writeBlob(const_cast<S*>(array_), size() * sizeof(S));
    }

    /// @returns flatten size (= product of shape vector)
    Size size() const { return size_; }

    /// Generic accessor
    /// Not very efficient, please refrain from using it in tight loops
    /// @note implements column-major (Fortran-style) ordering
    S& operator()(Size dim, ...) {
        ASSERT(shape_.size());
        Size idx  = dim;
        Size prod = shape_[0];
        int nargs = 1;
        va_list args;
        va_start(args, dim);
        for (Size i = 1; i < shape_.size(); ++i) {
            ++nargs;
            Size d = va_arg(args, Size);
            idx += d * prod;
            prod *= shape_[i];
        }
        va_end(args);
        ASSERT(nargs == shape_.size());
        ASSERT(idx < size());
        return array_[idx];
    }

    /// Access to linearised storage
    S& operator[](Size i) { return array_[i]; }
    const S& operator[](Size i) const { return array_[i]; }

    /// @returns modifiable view of the data
    S* data() { return array_; }
    /// @returns read-only view of the data
    const S* data() const { return array_; }

    /// @returns iterator to beginning of the data
    S* begin() { return array_; }
    /// @returns const iterator to beginning of the data
    const S* begin() const { return array_; }
    /// @returns iterator to end of the data
    S* end() { return array_ + size(); }
    /// @returns const iterator to end of the data
    const S* end() const { return array_ + size(); }

protected:  // member variables
    /// Container
    S* array_;

    /// flattened size
    Size size_;

    /// tensor shape is a vector of sizes per dimension
    std::vector<Size> shape_;

    /// Indicate ownership
    bool own_;
};

//----------------------------------------------------------------------------------------------------------------------

typedef Tensor<double> TensorDouble;
typedef Tensor<float>  TensorFloat;

//----------------------------------------------------------------------------------------------------------------------

template <typename S>
Stream& operator<<(Stream& s, const Tensor<S>& t) {
    t.encode(s);
    return s;
}

//----------------------------------------------------------------------------------------------------------------------


}  // namespace linalg
}  // namespace eckit
