/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/linalg/Matrix.h"

#include <cstring>
#include "eckit/io/Buffer.h"
#include "eckit/serialisation/Stream.h"

namespace eckit {
namespace linalg {

//-----------------------------------------------------------------------------

Matrix::Matrix() : m_(0),
    rows_(0),
    cols_(0),
    own_(false) {}

//-----------------------------------------------------------------------------

Matrix::Matrix(Size rows, Size cols) :
    m_(new Scalar[rows*cols]),
    rows_(rows),
    cols_(cols),
    own_(true) {
    ASSERT(size()>0);
    ASSERT(m_);
}

//-----------------------------------------------------------------------------

Matrix::Matrix(Scalar* m, Size rows, Size cols) :
    m_(m),
    rows_(rows),
    cols_(cols),
    own_(false) {
    ASSERT(size()>0);
    ASSERT(m_);
}

//-----------------------------------------------------------------------------

Matrix::Matrix(Stream& stream) :
    m_(0),
    rows_(0),
    cols_(0),
    own_(false) {
    Size rows, cols;
    resize(rows, cols);

    ASSERT(size()>0);
    ASSERT(m_);
    Buffer b(m_, (rows*cols)*sizeof(Scalar), /* dummy */ true);

    stream >> rows;
    stream >> cols;
    stream >> b;
}

//-----------------------------------------------------------------------------

Matrix::Matrix(const Matrix& other) :
    m_(new Scalar[other.size()]),
    rows_(other.rows_),
    cols_(other.cols_),
    own_(true) {
    ASSERT(size()>0);
    ASSERT(m_);
    ::memcpy(m_, other.m_, size() * sizeof(Scalar));
}

//-----------------------------------------------------------------------------

Matrix::~Matrix() {
    if (own_) {
        delete [] m_;
    }
}

//-----------------------------------------------------------------------------

Matrix& Matrix::operator=(const Matrix& other) {
    // do not optimize for if size()==other.size(), as using copy constructor
    // consistently retains ownership (no surprises in ownership behaviour)
    Matrix m(other);
    swap(m);
    return *this;
}

//-----------------------------------------------------------------------------

void Matrix::swap(Matrix& other) {
    std::swap(m_,    other.m_);
    std::swap(rows_, other.rows_);
    std::swap(cols_, other.cols_);
    std::swap(own_,  other.own_);
}

//-----------------------------------------------------------------------------

void Matrix::resize(Size rows, Size cols) {
    // avoid reallocation if memory is the same
    if (size() != rows*cols) {
        Matrix m(rows, cols);
        swap(m);
    }
    rows_ = rows;
    cols_ = cols;
}

//-----------------------------------------------------------------------------

void Matrix::setZero() {
    ASSERT(size()>0);
    ASSERT(m_);
    ::memset(m_, 0, size()*sizeof(Scalar));
}

//-----------------------------------------------------------------------------

void Matrix::fill(Scalar value) {
    for (Size i = 0; i < size(); ++i) {
        m_[i] = value;
    }
}

//-----------------------------------------------------------------------------

void Matrix::encode(Stream& stream) const {
  Buffer b(const_cast<Scalar*>(m_), rows_*cols_*sizeof(Scalar), /* dummy */ true);

  stream << rows_;
  stream << cols_;
  stream << b;
}

//-----------------------------------------------------------------------------

Stream& operator<<(Stream& stream, const Matrix& matrix) {
    matrix.encode(stream);
    return stream;
}

//-----------------------------------------------------------------------------

}  // namespace linalg
}  // namespace eckit
