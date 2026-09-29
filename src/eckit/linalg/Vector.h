// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

/// @author Florian Rathgeber
/// @author Pedro Maciel
/// @author Tiago Quintino


#pragma once

#include "eckit/linalg/types.h"


namespace eckit {
class Stream;
}

namespace eckit::linalg {

//----------------------------------------------------------------------------------------------------------------------

/// Vector for Linear Algebra operations
/// @todo provide a const view
class Vector {

public:  // methods

    // -- Constructors

    /// Default constructor (empty vector)
    Vector();

    /// Construct vector of given size (allocates memory, not initialised)
    Vector(Size length);

    /// Construct vector from existing data (does NOT take ownership)
    Vector(const Scalar array[], Size length);

    /// Constructor from Stream
    Vector(Stream&);

    /// Copy constructor
    Vector(const Vector&);

    ~Vector();

    // -- Mutators

    Vector& operator=(const Vector&);

    /// Swap this vector for another
    void swap(Vector&);

    /// Resize vector to given size (invalidates data)
    void resize(Size length);

    /// Set data to zero
    void setZero();

    /// Fill vector with given scalar
    void fill(Scalar);

    // -- Serialisation

    /// Serialise to a Stream
    void encode(Stream&) const;

    // -- Accessors

    /// @returns size (rows * cols)
    Size size() const { return length_; }

    /// @returns number of rows (i.e. size)
    Size rows() const { return length_; }

    /// @returns number of columns (always 1)
    Size cols() const { return 1; }

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
    Scalar* end() { return array_ + length_; }

    /// @returns const iterator to end of the data
    const Scalar* end() const { return array_ + length_; }

protected:  // member variables

    Scalar* array_;  ///< Container
    Size length_;    ///< Vector length/size
    bool own_;       ///< do we own the memory allocated in the container ?
};


Stream& operator<<(Stream&, const Vector&);


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::linalg
