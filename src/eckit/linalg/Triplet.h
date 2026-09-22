// SPDX-FileCopyrightText: 1996- European Centre for Medium-Range Weather Forecasts (ECMWF)
// SPDX-License-Identifier: Apache-2.0

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
