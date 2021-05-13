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

#include <functional>
#include <numeric>
#include <vector>

#include "eckit/linalg/types.h"

namespace eckit {
class Stream;
}

namespace eckit {
namespace linalg {

//-----------------------------------------------------------------------------

Size flatten_shape(const std::vector<Size>& shape) {
    return std::accumulate(std::begin(shape), std::end(shape), 1, std::multiplies<Size>());
}

//-----------------------------------------------------------------------------

/// Dense Tensor in column major storage order
class Tensor {
public:  // types
    typedef eckit::linalg::Size Size;

public:  // methods
    /// Default constructor (empty tensor)
    Tensor();

    /// Construct tensor with given rows and columns (allocates memory, not initialised)
    Tensor(const std::vector<Size>& shape);

    /// Construct tensor from existing data (does NOT take ownership)
    Tensor(const Scalar* array, const std::vector<Size>& shape);

    /// Constructor from Stream
    Tensor(Stream&);

    /// Copy constructor
    Tensor(const Tensor&);

    ~Tensor();

    // -- Mutators

    Tensor& operator=(const Tensor&);

    /// Swap this tensor with another
    void swap(Tensor&);

    /// Resize tensor to given a shape (invalidates data)
    void resize(const std::vector<Size>& shape);

    /// Set data to zero
    void setZero();

    /// Fill vector with given scalar
    void fill(Scalar);

    // -- Serialisation

    /// Serialise to a Stream
    void encode(Stream&) const;

    // -- Accessors

    /// @returns flatten size (= product of shape vector)
    Size size() const { return size_; }

    /// Access by row and column
    /// @note implements column-major (Fortran-style) ordering
    // Scalar& operator()(Size row, Size col) { return array_[col * rows_ + row]; }
    // const Scalar& operator()(Size row, Size col) const { return array_[col * rows_ + row]; }

    /// Access to linearised storage
    Scalar& operator[](Size i) { return array_[i]; }
    const Scalar& operator[](Size i) const { return array_[i]; }

    /// @returns modifiable view of the data
    Scalar* data() { return array_; }
    /// @returns read-only view of the data
    const Scalar* data() const { return array_; }

    /// @returns iterator to beginning of the data
    Scalar* begin() { return array_; }
    /// @returns const iterator to beginning of the data
    const Scalar* begin() const { return array_; }
    /// @returns iterator to end of the data
    Scalar* end() { return array_ + size(); }
    /// @returns const iterator to end of the data
    const Scalar* end() const { return array_ + size(); }

protected:  // member variables
    /// Container
    Scalar* array_;

    /// flattened size
    Size size_;

    /// tensor shape is a vector of sizes per dimension
    std::vector<Size> shape_;

    /// Indicate ownership
    bool own_;
};

//-----------------------------------------------------------------------------

Stream& operator<<(Stream&, const Tensor&);

//-----------------------------------------------------------------------------

}  // namespace linalg
}  // namespace eckit
