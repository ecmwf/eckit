/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "eckit/linalg/Matrix.h"

#include <cstring>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/Buffer.h"
#include "eckit/serialisation/Stream.h"

namespace eckit {
namespace linalg {

//----------------------------------------------------------------------------------------------------------------------

Matrix::Matrix() :
    array_(0), rows_(0), cols_(0), own_(false) {}


Matrix::Matrix(Size rows, Size cols) :
    array_(new Scalar[rows * cols]), rows_(rows), cols_(cols), own_(true) {
    ASSERT(size() > 0);
    ASSERT(array_);
}


Matrix::Matrix(const Scalar* array, Size rows, Size cols) :
    array_(const_cast<Scalar*>(array)), rows_(rows), cols_(cols), own_(false) {
    ASSERT(size() > 0);
    ASSERT(array_);
}


Matrix::Matrix(Stream& stream) :
    array_(0), rows_(0), cols_(0), own_(false) {
    Size rows, cols;
    stream >> rows;
    stream >> cols;
    resize(rows, cols);

    ASSERT(size() > 0);
    ASSERT(array_);
    stream.readBlob(array_, (rows * cols) * sizeof(Scalar));
}


Matrix::Matrix(const Matrix& other) :
    array_(new Scalar[other.size()]), rows_(other.rows_), cols_(other.cols_), own_(true) {
    ASSERT(size() > 0);
    ASSERT(array_);
    ::memcpy(array_, other.array_, size() * sizeof(Scalar));
}


Matrix::~Matrix() {
    if (own_) {
        delete[] array_;
    }
}


Matrix& Matrix::operator=(const Matrix& other) {
    // do not optimize for if size()==other.size(), as using copy constructor
    // consistently retains ownership (no surprises in ownership behaviour)
    Matrix copy(other);
    swap(copy);
    return *this;
}


void Matrix::swap(Matrix& other) {
    std::swap(array_, other.array_);
    std::swap(rows_, other.rows_);
    std::swap(cols_, other.cols_);
    std::swap(own_, other.own_);
}


void Matrix::resize(Size rows, Size cols) {
    // avoid reallocation if memory is the same
    if (size() != rows * cols) {
        Matrix m(rows, cols);
        swap(m);
    }
    rows_ = rows;
    cols_ = cols;
}


void Matrix::setZero() {
    ASSERT(size() > 0);
    ASSERT(array_);
    ::memset(array_, 0, size() * sizeof(Scalar));
}


void Matrix::fill(Scalar value) {
    for (Size i = 0; i < size(); ++i) {
        array_[i] = value;
    }
}


void Matrix::encode(Stream& stream) const {
    stream << rows_;
    stream << cols_;
    stream.writeBlob(const_cast<Scalar*>(array_), rows_ * cols_ * sizeof(Scalar));
}


Stream& operator<<(Stream& stream, const Matrix& matrix) {
    matrix.encode(stream);
    return stream;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace linalg
}  // namespace eckit
