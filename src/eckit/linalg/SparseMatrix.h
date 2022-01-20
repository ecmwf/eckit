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
/// @author Tiago Quintino


#pragma once

#include <cassert>
#include <iosfwd>
#include <memory>
#include <vector>

#include "eckit/io/MemoryHandle.h"
#include "eckit/linalg/Triplet.h"
#include "eckit/linalg/types.h"
#include "eckit/memory/MemoryBuffer.h"
#include "eckit/memory/NonCopyable.h"


namespace eckit {
class Stream;
class PathName;
}  // namespace eckit


namespace eckit {
namespace linalg {


//----------------------------------------------------------------------------------------------------------------------


/// Sparse matrix in CRS (compressed row storage) format
class SparseMatrix {
public:  // types
    struct Layout {

        Layout() :
            data_(nullptr), outer_(nullptr), inner_(nullptr) {}

        void reset() {
            data_  = nullptr;
            outer_ = nullptr;
            inner_ = nullptr;
        }

        Scalar* data_;  ///< matrix entries, sized with number of non-zeros (nnz)
        Index* outer_;  ///< start of rows,  sized number of rows + 1
        Index* inner_;  ///< column indices, sized with number of non-zeros (nnz)
    };


    struct Shape {

        Shape() :
            size_(0), rows_(0), cols_(0) {}

        void reset() {
            size_ = 0;
            rows_ = 0;
            cols_ = 0;
        }

        /// @returns number of rows
        Size rows() const { return rows_; }

        /// @returns number of columns
        Size cols() const { return cols_; }

        /// @returns number of non-zeros
        Size nonZeros() const { return size_; }

        /// data size is the number of non-zeros
        Size dataSize() const { return nonZeros(); }

        /// inner size is the number of non-zeros
        Size innerSize() const { return nonZeros(); }

        /// @returns outer size is number of rows + 1
        Size outerSize() const { return Size(rows_ + 1); }

        size_t allocSize() const { return sizeofData() + sizeofOuter() + sizeofInner(); }

        size_t sizeofData() const { return dataSize() * sizeof(Scalar); }
        size_t sizeofOuter() const { return outerSize() * sizeof(Index); }
        size_t sizeofInner() const { return innerSize() * sizeof(Index); }

        Size size_;  ///< Size of the container (AKA number of non-zeros nnz)
        Size rows_;  ///< Number of rows
        Size cols_;  ///< Number of columns

        void print(std::ostream& os) const {
            os << "Shape["
               << "nnz=" << size_ << ","
               << "rows=" << rows_ << ","
               << "cols=" << cols_ << "]";
        }

        friend std::ostream& operator<<(std::ostream& os, const Shape& p) {
            p.print(os);
            return os;
        }
    };


    class Allocator {
    public:
        virtual ~Allocator();

        /// @note that shape may be modified by the allocator, e.g. loading of pre-computed matrices
        virtual Layout allocate(Shape&) = 0;

        /// Layout and Shape parameters may be ignored
        virtual void deallocate(Layout, Shape) = 0;

        /// Is the memory shared
        virtual bool inSharedMemory() const = 0;

        virtual void print(std::ostream&) const = 0;

        friend std::ostream& operator<<(std::ostream& os, const Allocator& a) {
            a.print(os);
            return os;
        }
    };

public:  // methods
    // -- Constructors

    /// Default constructor, empty matrix
    SparseMatrix(Allocator* alloc = nullptr);

    /// Constructs an identity matrix with provided dimensions
    SparseMatrix(Size rows, Size cols, Allocator* alloc = nullptr);

    /// Constructor from triplets
    SparseMatrix(Size rows, Size cols, const std::vector<Triplet>& triplets);

    /// Constructor from Stream
    SparseMatrix(Stream& v);

    /// Constructor from MemoryBuffer
    SparseMatrix(const MemoryBuffer&);

    /// Move constructor
    SparseMatrix(SparseMatrix&&);

    /// Copy constructor
    SparseMatrix(const SparseMatrix&);

    ~SparseMatrix();

    /// Assignment operator (allocates and copies data)
    SparseMatrix& operator=(const SparseMatrix&);

public:
    /// Prune entries with exactly the given value
    SparseMatrix& prune(Scalar val = Scalar(0));

    /// Set matrix to the identity
    SparseMatrix& setIdentity(Size rows, Size cols);

    /// Transpose matrix in-place
    SparseMatrix& transpose();

    /// @returns a sparse matrix that is a row reduction and reorder accoring to indexes passed in vector
    SparseMatrix rowReduction(const std::vector<size_t>& p) const;

    // -- I/O

    void save(const eckit::PathName& path) const;
    void load(const eckit::PathName& path);

    void dump(eckit::MemoryBuffer& buffer) const;
    void dump(void* buffer, size_t size) const;

    static void load(const void* buffer, size_t bufferSize, Layout& layout, Shape& shape);  ///< from dump()

    void swap(SparseMatrix& other);

    /// @returns number of rows
    Size rows() const { return shape_.rows_; }

    /// @returns number of columns
    Size cols() const { return shape_.cols_; }

    /// @returns number of non-zeros
    Size nonZeros() const { return shape_.size_; }

    /// @returns true if this matrix does not contain non-zero entries
    bool empty() const { return !nonZeros(); }

    /// @returns read-only view of the data vector
    const Scalar* data() const { return spm_.data_; }

    /// @returns read-only view of the outer index vector
    const Index* outer() const { return spm_.outer_; }

    /// @returns read-only view of the inner index vector
    const Index* inner() const { return spm_.inner_; }

    /// Set a new number of columns (may invalidate data)
    void cols(Size cols);

    /// Reserve memory for given number of non-zeros (invalidates all data arrays)
    /// @note variables into this method must be by value
    void reserve(Size rows, Size cols, Size nnz);

    /// Returns the footprint of the matrix in memory
    size_t footprint() const;

    /// Is the memory shared
    bool inSharedMemory() const;

    void dump(std::ostream&) const;

    void print(std::ostream&) const;

    const Allocator& owner() const;

    friend std::ostream& operator<<(std::ostream& os, const SparseMatrix& m) {
        m.print(os);
        return os;
    }

public:  // iterators
    struct const_iterator {

        const_iterator(const SparseMatrix& matrix);
        const_iterator(const SparseMatrix& matrix, Size row);

        const_iterator(const const_iterator& other) { *this = other; }

        Size col() const;
        Size row() const;

        operator bool() const { return matrix_ && (index_ < matrix_->nonZeros()); }

        const_iterator& operator++();
        const_iterator operator++(int);
        const_iterator& operator=(const const_iterator& other);

        bool operator!=(const const_iterator& other) const { return !operator==(other); }
        bool operator==(const const_iterator& other) const;

        const Scalar& operator*() const;

        void print(std::ostream&) const;

        bool lastOfRow() const { return ((index_ + 1) == Size(matrix_->outer()[row_ + 1])); }

    protected:
        SparseMatrix* matrix_;
        Size index_;
        Size row_;
    };

    struct iterator : const_iterator {
        iterator(SparseMatrix& matrix) :
            const_iterator(matrix) {}
        iterator(SparseMatrix& matrix, Size row) :
            const_iterator(matrix, row) {}
        Scalar& operator*();
    };

    /// const iterators to begin/end of row
    const_iterator begin(Size row) const { return const_iterator(*this, row); }
    const_iterator end(Size row) const { return const_iterator(*this, row + 1); }

    /// const iterators to begin/end of matrix
    const_iterator begin() const { return const_iterator(*this); }
    const_iterator end() const { return const_iterator(*this, rows()); }

    /// iterators to begin/end of row
    iterator begin(Size row) { return iterator(*this, row); }
    iterator end(Size row) { return iterator(*this, row + 1); }

    /// const iterators to begin/end of matrix
    iterator begin() { return iterator(*this); }
    iterator end() { return iterator(*this, rows()); }

private:  // methods
    /// Resets the matrix to a deallocated state
    void reset();

    /// Serialise to a Stream
    void encode(Stream& s) const;

    /// Deserialise from a Stream
    void decode(Stream& s);

private:  // members
    Layout spm_;

    Shape shape_;

    std::unique_ptr<SparseMatrix::Allocator> owner_;  ///< memory manager / allocator

    friend Stream& operator<<(Stream&, const SparseMatrix&);
};


Stream& operator<<(Stream&, const SparseMatrix&);


//----------------------------------------------------------------------------------------------------------------------


}  // namespace linalg
}  // namespace eckit
