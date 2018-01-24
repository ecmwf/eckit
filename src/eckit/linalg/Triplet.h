/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

/// @file   Triplet.h
/// @author Florian Rathgeber
/// @date   June 2015

#ifndef eckit_la_Triplet_h
#define eckit_la_Triplet_h

#include <iosfwd>

#include "eckit/linalg/types.h"

namespace eckit {
namespace linalg {

//----------------------------------------------------------------------------------------------------------------------

/// Triplet of values compatible to Eigen::Triplet

class Triplet {
public:

    Triplet() : row_(0), col_(0), val_(0) {}

    Triplet(const Size& i, const Size& j, const Scalar& v = Scalar(0))
        : row_(i), col_(j), val_(v)
    {}

    const Size& row() const { return row_; }

    const Size& col() const { return col_; }

    const Scalar& value() const { return val_; }
    Scalar& value() { return val_; }

    bool operator< (const Triplet& other) const
    {
        if(row_ == other.row_)
        {
            return col_ < other.col_;
        }
        return row_ < other.row_;
    }

    void print(std::ostream& os) const;

    friend std::ostream& operator<<(std::ostream& os, const Triplet& p);

protected:
    Size row_;
    Size col_;
    Scalar val_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace linalg
} // namespace eckit

#endif
