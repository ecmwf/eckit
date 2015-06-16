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

#include <vector>

#include "eckit/la/types.h"

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

class Vector {
public:  // types
    typedef std::vector<Scalar> Storage;
    typedef Storage::size_type Index;

public:  // methods

    // -- Constructors

    /// Default constructor: empty vector
    Vector() {}

    /// Construct vector of given size (allocates memory)
    Vector(Index s) : v_(s) {}

    /// Construct vector from existing data (creates a copy)
    Vector(const Storage& v) : v_(v) {}

    /// @returns size (rows * cols)
    Index size() const { return v_.size(); }
    /// @returns number of rows (i.e. size)
    Index rows() const { return v_.size(); }
    /// @returns number of columns (always 1)
    Index cols() const { return 1; }

    Scalar& operator[](Index i) { return v_[i]; }
    const Scalar& operator[](Index i) const { return v_[i]; }

    /// @returns modifiable view of the data
    Scalar* data() { return v_.data(); }
    /// @returns read-only view of the data
    const Scalar* data() const { return v_.data(); }

protected:  // members
    Storage v_;
};

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit

#endif
