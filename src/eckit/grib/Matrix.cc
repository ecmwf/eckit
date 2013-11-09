// File Matrix.cc
// Baudouin Raoult - (c) ECMWF Nov 13

#include "Matrix.h"
#include "eckit/exception/Exceptions.h"


namespace eckit {

Matrix::Matrix(size_t rows, size_t cols):
    rows_(rows), cols_(cols), shared_(0)
{
    values_ = new double[rows*cols];
    ASSERT(values_);
}

Matrix::Matrix(const Matrix& other):
    rows_(other.rows_),
    cols_(other.cols_),
    shared_(other.shared_),
    values_(other.values_)
{
    if(!shared_) {
        shared_ = const_cast<Matrix&>(other).shared_ = new size_t(1);
    }
    (*shared_)++;
}

Matrix& Matrix::operator=(const Matrix& other)
{
    if(&other != this) {
        cleanup();
        rows_ = other.rows_;
        cols_ = other.cols_;
        shared_ = other.shared_;
        values_= other.values_;

        if(!shared_) {
            shared_ = const_cast<Matrix&>(other).shared_ = new size_t(1);
        }
        (*shared_)++;

    }
    return *this;
}

Matrix::~Matrix()
{
    cleanup();
}

void Matrix::cleanup() {
    if(shared_) {
        ASSERT(*shared_);
        (*shared_)--;
        if((*shared_) == 0) {
            delete shared_;
            delete values_;
        }
        shared_ = 0;
        values_ = 0;
    }
    else {
        delete values_;
        values_ = 0;
    }
}

void Matrix::print(ostream& os) const
{
    os << "Matrix[" << rows_ << "," << cols_ << "]";
}

void Matrix::copy() {
    ASSERT(shared_);
    shared_--;
}

} // namespace
