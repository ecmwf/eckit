/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include <numeric>

#include "eckit/io/Buffer.h"
#include "eckit/exception/Exceptions.h"
#include "eckit/serialisation/Stream.h"
#include "eckit/io/BufferedHandle.h"
#include "eckit/filesystem/PathName.h"

#include "experimental/eckit/la/SparseMatrix.h"

namespace eckit {
namespace la {

//-----------------------------------------------------------------------------

SparseMatrix::SparseMatrix() {}

//-----------------------------------------------------------------------------

SparseMatrix::SparseMatrix(SparseMatrix::Size rows, SparseMatrix::Size cols)
  : rows_(rows), cols_(cols) {}

//-----------------------------------------------------------------------------

// SparseMatrix::SparseMatrix(SparseMatrix::Size rows, SparseMatrix::Size cols, SparseMatrix::Size nnz)
//   : v_(nnz), outer_(rows+1), inner_(nnz), rows_(rows), cols_(cols) {}

// //-----------------------------------------------------------------------------

// SparseMatrix::SparseMatrix(SparseMatrix::Size rows, SparseMatrix::Size cols,
//                            const SparseMatrix::ScalarStorage& v,
//                            const SparseMatrix::IndexStorage& outer,
//                            const SparseMatrix::IndexStorage& inner)
//   : v_(v), rows_(rows), cols_(cols) {
//     ASSERT(v.size() == inner.size() && outer.size() == rows+1);
// }

//-----------------------------------------------------------------------------

SparseMatrix::SparseMatrix(Stream& s) {
    s >> rows_;
    s >> cols_;
    Index nnz;
    s >> nnz;
    reserve(nnz);
    Buffer outer(const_cast<Index*>(outer_.data()), (rows_+1)*sizeof(Index), /* dummy */ true);
    Buffer inner(const_cast<Index*>(inner_.data()), nnz*sizeof(Index), /* dummy */ true);
    Buffer data(const_cast<Scalar*>(v_.data()), nnz*sizeof(Scalar), /* dummy */ true);
    s >> outer;
    s >> inner;
    s >> data;
}

//-----------------------------------------------------------------------------

void SparseMatrix::resize(SparseMatrix::Size rows, SparseMatrix::Size cols) {
    v_.clear();
    outer_.clear();
    inner_.clear();
    rows_ = rows;
    cols_ = cols;
}

//-----------------------------------------------------------------------------

void SparseMatrix::reserve(SparseMatrix::Size nnz) {
    // Rows and columns must have been set before
    ASSERT( rows_ > 0 && cols_ > 0 );
    outer_.resize(rows_+1);
    inner_.resize(nnz);
    v_.resize(nnz);
}

//-----------------------------------------------------------------------------

void SparseMatrix::swap(SparseMatrix& other) {
    std::swap(v_, other.v_);
    std::swap(outer_, other.outer_);
    std::swap(inner_, other.inner_);
    std::swap(rows_, other.rows_);
    std::swap(cols_, other.cols_);
}

//-----------------------------------------------------------------------------

void SparseMatrix::setFromTriplets(const std::vector<Triplet>& triplets) {
    // Allocate memory (we are promised that there is 1 triplet per non-zero)
    reserve(triplets.size());

    Size pos = 0;
    Index row = -1;
    // Build vectors of inner indices and values, update outer index per row
    for (std::vector<Triplet>::const_iterator it = triplets.begin(); it != triplets.end(); ++it, ++pos) {
        // We are promised triplets are ordered by rows
        ASSERT( it->row() >= row );
        ASSERT( it->row() < rows_ );
        ASSERT( it->col() >= 0 );
        ASSERT( it->col() < cols_ );
        // We start a new row
        while (it->row() > row) {
            outer_[++row] = Index(pos);
        }
        inner_[pos] = it->col();
        v_[pos] = it->value();
    }

    while (row < rows_) {
        outer_[++row] = Index(pos);
    }
}

//-----------------------------------------------------------------------------

void SparseMatrix::setIdentity() {
    Index nnz = std::min(rows_, cols_);
    outer_.resize(rows_+1);
    inner_.resize(nnz);
    for (Index i = 0; i < nnz; ++i) {
        outer_[i] = i;
        inner_[i] = i;
    }
    for (Index i = nnz; i <= rows_; ++i) {
        outer_[i] = nnz;
    }
    v_.assign(nnz, Scalar(1));
}

//-----------------------------------------------------------------------------

void SparseMatrix::prune(SparseMatrix::Scalar val) {
    ScalarStorage v;
    IndexStorage outer;
    IndexStorage inner;
    Index nnz = 0;
    for (Index r = 0; r < rows_; ++r) {
        outer.push_back(nnz);
        for (Index c = outer_[r]; c < outer_[r+1]; ++c)
            if (v_[c] != val) {
                v.push_back(v_[c]);
                inner.push_back(inner_[c]);
                ++nnz;
            }
    }
    outer.push_back(nnz);
    std::swap(v, v_);
    std::swap(outer, outer_);
    std::swap(inner, inner_);
}

//-----------------------------------------------------------------------------

void SparseMatrix::encode(Stream& s) const {
    const Index nnz = outer_[rows_];
    s << rows_;
    s << cols_;
    s << nnz;
    s << Buffer(const_cast<Index*>(outer_.data()), (rows_+1)*sizeof(Index), /* dummy */ true);
    s << Buffer(const_cast<Index*>(inner_.data()), nnz*sizeof(Index), /* dummy */ true);
    s << Buffer(const_cast<Scalar*>(v_.data()), nnz*sizeof(Scalar), /* dummy */ true);
}

//-----------------------------------------------------------------------------

Stream& operator<<(Stream& s, const SparseMatrix& v) {
    v.encode(s);
    return s;
}

SparseMatrix::_InnerIterator::_InnerIterator(SparseMatrix& m, SparseMatrix::Index outer)
    : m_(m), outer_(outer), inner_(m_.outer_[outer])
{
    ASSERT(outer >= 0);
    ASSERT(outer < m.rows_);
    ASSERT(inner_ >= 0);
    ASSERT(inner_ <= m_.v_.size());
}

SparseMatrix::Scalar SparseMatrix::_InnerIterator::operator*() const {
    ASSERT(inner_ < m_.v_.size());
    return m_.v_[inner_];
}

SparseMatrix::Scalar& SparseMatrix::InnerIterator::operator*() {
    ASSERT(inner_ < m_.v_.size());
    return m_.v_[inner_];
}

//-----------------------------------------------------------------------------

void SparseMatrix::save(const eckit::PathName &path) const {
    eckit::BufferedHandle f(path.fileHandle());

    f.openForWrite(0);
    eckit::AutoClose closer(f);

    // write nominal size of matrix

    Index iSize = innerSize();
    Index oSize = outerSize();

    f.write(&iSize, sizeof(iSize));
    f.write(&oSize, sizeof(oSize));

    // find all the non-zero values (aka triplets)

    std::vector<Triplet > trips;
    for (size_t i = 0; i < outerSize(); ++i) {
        for (ConstInnerIterator it(*this, i); it; ++it) {
            trips.push_back(Triplet(it.row(), it.col(), *it));
        }
    }

    // save the number of triplets

    Index ntrips = trips.size();
    f.write(&ntrips, sizeof(ntrips));

    // now save the triplets themselves

    for (size_t i = 0; i < trips.size(); i++) {

        Triplet &rt = trips[i];

        Index x = rt.row();
        Index y = rt.col();
        double w = rt.value();

        f.write(&x, sizeof(x));
        f.write(&y, sizeof(y));
        f.write(&w, sizeof(w));
    }
}

void SparseMatrix::load(const eckit::PathName &path)  {

    eckit::BufferedHandle f(path.fileHandle());

    f.openForRead();
    eckit::AutoClose closer(f);

    // read inpts, outpts sizes of matrix

    Index inner, outer;

    f.read(&inner, sizeof(inner));
    f.read(&outer, sizeof(outer));

    Index npts;
    f.read(&npts, sizeof(npts));

    // read total sparse points of matrix (so we can reserve)

    std::vector<Triplet > insertions;

    insertions.reserve(npts);

    // read the values

    for (size_t i = 0; i < npts; i++) {
        Index x, y;
        double w;
        f.read(&x, sizeof(x));
        f.read(&y, sizeof(y));
        f.read(&w, sizeof(w));
        insertions.push_back(Triplet(x, y, w));
    }

    // check matrix is correctly sized
    // note that Weigths::Matrix is row-major, so rows are outer size

    ASSERT(rows() == outer);
    ASSERT(cols() == inner);

    // set the weights from the triplets

    setFromTriplets(insertions);
}

//-----------------------------------------------------------------------------

}  // namespace la
}  // namespace eckit
