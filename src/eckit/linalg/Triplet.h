/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @author Florian Rathgeber
/// @author Pedro Maciel


#pragma once

#include <iosfwd>

#include "eckit/linalg/types.h"

namespace eckit::linalg {

//----------------------------------------------------------------------------------------------------------------------

/// Triplet of values compatible to Eigen::Triplet
class Triplet {
public:

    Triplet() : row_(0), col_(0), val_(0.) {}

    Triplet(const Size& i, const Size& j, const Scalar& v = {0}) : row_(i), col_(j), val_(v) {}

    const Size& row() const { return row_; }

    const Size& col() const { return col_; }

    const Scalar& value() const { return val_; }

    Scalar& value() { return val_; }

    bool operator<(const Triplet& other) const { return row_ == other.row_ ? col_ < other.col_ : row_ < other.row_; }

    bool nonZero() const { return val_ != 0.; }

    void print(std::ostream&) const;

    friend std::ostream& operator<<(std::ostream&, const Triplet&);

protected:

    Size row_;
    Size col_;
    Scalar val_;
};


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::linalg
