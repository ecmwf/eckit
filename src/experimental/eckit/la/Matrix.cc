/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/io/Buffer.h"
#include "eckit/serialisation/Stream.h"

#include "eckit/la/Matrix.h"

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

Matrix::Matrix() {}

//-----------------------------------------------------------------------------

Matrix::Matrix(Matrix::Size rows, Matrix::Size cols)
  : v_(rows*cols), rows_(rows), cols_(cols) {}

//-----------------------------------------------------------------------------

Matrix::Matrix(Stream& s) {
    Size rows, cols;
    s >> rows;
    s >> cols;
    resize(rows, cols);
    Buffer b(v_.data(), rows*cols*sizeof(Scalar), /* dummy */ true);
    s >> b;
}

//-----------------------------------------------------------------------------

void Matrix::resize(Matrix::Size rows, Matrix::Size cols) {
    v_.resize(rows*cols);
    rows_ = rows;
    cols_ = cols;
}

//-----------------------------------------------------------------------------

void Matrix::swap(Matrix& o) {
    v_.swap(o.v_);
    std::swap(rows_, o.rows_);
    std::swap(cols_, o.cols_);
}

//-----------------------------------------------------------------------------

void Matrix::encode(Stream& s) const {
  s << rows_;
  s << cols_;
  s << Buffer(const_cast<Scalar*>(v_.data()), rows_*cols_*sizeof(Scalar), /* dummy */ true);
}

//-----------------------------------------------------------------------------

Stream& operator<<(Stream& s, const Matrix& v) {
    v.encode(s);
    return s;
}

//-----------------------------------------------------------------------------

}  // namespace la
} // namespace eckit
