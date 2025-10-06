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

#include <iosfwd>
#include <memory>
#include <type_traits>
#include <vector>

#include "eckit/linalg/Triplet.h"
#include "eckit/linalg/types.h"


namespace eckit {
class MemoryBuffer;
class PathName;
class Stream;
}  // namespace eckit

namespace eckit::linalg {

//----------------------------------------------------------------------------------------------------------------------

/// Sparse matrix in CRS (compressed row storage) format
class SparseMatrix {
public:

    using UIndex = std::make_unsigned_t<Index>;


    struct Layout {
        Layout() = default;

        void reset() {
            data_  = nullptr;
            outer_ = nullptr;
            inner_ = nullptr;
        }

        Scalar* data_  = nullptr;  ///< matrix entries, sized with number of non-zeros (nnz)
        UIndex* outer_ = nullptr;  ///< start of rows, sized number of rows + 1
        Index* inner_  = nullptr;  ///< column indices, sized with number of non-zeros (nnz)
    };


    struct Shape {
        Shape() = default;

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

        /// @returns number of non-zeros
        Size dataSize() const { return nonZeros(); }

        /// @returns number of non-zeros
        Size innerSize() const { return nonZeros(); }

        /// @returns outer size is number of rows + 1
        Size outerSize() const { return rows_ + 1; }

        size_t allocSize() const { return sizeofData() + sizeofOuter() + sizeofInner(); }

        size_t sizeofData() const { return dataSize() * sizeof(Scalar); }
        size_t sizeofOuter() const { return outerSize() * sizeof(UIndex); }
        size_t sizeofInner() const { return innerSize() * sizeof(Index); }

        Size size_ = 0;  ///< Size of the container (AKA number of non-zeros nnz)
        Size rows_ = 0;  ///< Number of rows
        Size cols_ = 0;  ///< Number of columns

        void print(std::ostream&) const;

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

        virtual void deallocate(Layout, Shape) = 0;

        /// @returns if allocation is in shared memory
        virtual bool inSharedMemory() const = 0;

        virtual void print(std::ostream&) const = 0;

        friend std::ostream& operator<<(std::ostream& os, const Allocator& a) {
            a.print(os);
            return os;
        }
    };

public:

    // -- Constructors

    /// Default constructor, empty matrix
    explicit SparseMatrix(Allocator* = nullptr);

    /// Constructs an identity matrix with provided dimensions
    SparseMatrix(Size rows, Size cols, Allocator* = nullptr);

    /// Constructor from triplets
    SparseMatrix(Size rows, Size cols, const std::vector<Triplet>&);

    /// Constructor from Stream
    explicit SparseMatrix(Stream&);

    /// Constructor from MemoryBuffer
    explicit SparseMatrix(const MemoryBuffer&);

    /// Move constructor
    SparseMatrix(SparseMatrix&&);

    /// Copy constructor
    SparseMatrix(const SparseMatrix&);

    /// Destructor
    ~SparseMatrix();

    /// Assignment operator (allocates and copies data)
    SparseMatrix& operator=(const SparseMatrix&);

    /// Assignment operator (moves data)
    SparseMatrix& operator=(SparseMatrix&&);

public:

    /// Prune entries with exactly the given value
    SparseMatrix& prune(Scalar = 0);

    /// Set matrix to the identity
    SparseMatrix& setIdentity(Size rows, Size cols);

    /// Transpose matrix in-place
    SparseMatrix& transpose();

    /// @returns a sparse matrix that is a row reduction and reorder accoring to indexes passed in vector
    SparseMatrix rowReduction(const std::vector<size_t>& p) const;

    // -- I/O

    void save(const eckit::PathName&) const;
    void load(const eckit::PathName&);

    void dump(eckit::MemoryBuffer&) const;
    void dump(void* buffer, size_t size) const;

    static void load(const void* buffer, size_t bufferSize, Layout&, Shape&);  ///< from dump()

    void swap(SparseMatrix&);

    /// @returns number of rows
    Size rows() const { return shape_.rows_; }

    /// @returns number of columns
    Size cols() const { return shape_.cols_; }

    /// @returns number of non-zeros
    Size nonZeros() const { return shape_.size_; }

    /// @returns true if this matrix does not contain non-zero entries
    bool empty() const { return nonZeros() == 0; }

    /// @returns read-only view of the outer index vector
    const UIndex* outerIndex() const { return spm_.outer_; }

    /// @returns read-only view of the data vector
    const Scalar* data() const { return spm_.data_; }

    /// @returns read-only view of the outer index vector (signed)
    const Index* outer() const;

    /// @returns read-only view of the inner index vector
    const Index* inner() const { return spm_.inner_; }

    /// Set a new number of columns (may invalidate data)
    void cols(Size cols);

    /// Reserve memory for given number of non-zeros (invalidates all data arrays)
    void reserve(Size rows, Size cols, Size nnz);

    /// @returns footprint of the matrix in memory
    size_t footprint() const;

    /// @returns if allocation is in shared memory
    bool inSharedMemory() const;

    void dump(std::ostream&) const;

    void print(std::ostream&) const;

    const Allocator& owner() const;

    friend std::ostream& operator<<(std::ostream& os, const SparseMatrix& m) {
        m.print(os);
        return os;
    }

public:  // iterators

    struct iterator;

    struct const_iterator {
        const_iterator(const SparseMatrix&);
        const_iterator(const SparseMatrix&, Size row);

        const_iterator(const const_iterator&) = default;
        const_iterator(const_iterator&&)      = default;

        virtual ~const_iterator() = default;

        Size col() const;
        Size row() const;

        operator bool() const { return matrix_ && (index_ < matrix_->nonZeros()); }

        const_iterator& operator++();
        const_iterator operator++(int);

        const_iterator& operator=(const const_iterator&) = default;
        const_iterator& operator=(const_iterator&&)      = default;

        bool operator!=(const const_iterator& other) const { return !operator==(other); }
        bool operator==(const const_iterator&) const;

        const Scalar& operator*() const;

        void print(std::ostream&) const;

        bool lastOfRow() const { return ((index_ + 1) == static_cast<Size>(matrix_->spm_.outer_[row_ + 1])); }

    private:

        friend struct iterator;

        SparseMatrix* matrix_;
        Size index_;
        Size row_;
    };

    struct iterator final : const_iterator {
        using const_iterator::const_iterator;
        Scalar& operator*();
    };

    /// const iterators to begin/end of row
    const_iterator begin(Size row) const { return {*this, row}; }
    const_iterator end(Size row) const { return {*this, row + 1}; }

    /// const iterators to begin/end of matrix
    const_iterator begin() const { return {*this}; }
    const_iterator end() const { return {*this, rows()}; }

    /// iterators to begin/end of row
    iterator begin(Size row) { return {*this, row}; }
    iterator end(Size row) { return {*this, row + 1}; }

    /// const iterators to begin/end of matrix
    iterator begin() { return {*this}; }
    iterator end() { return {*this, rows()}; }

private:

    /// Resets the matrix to a deallocated state
    void reset();

    /// Serialise to a Stream
    void encode(Stream&) const;

    /// Deserialise from a Stream
    void decode(Stream&);

private:

    Layout spm_;  ///< Matrix layout

    Shape shape_;  ///< Matrix shape

    std::unique_ptr<SparseMatrix::Allocator> owner_;  ///< Matrix memory manager/allocator

    friend Stream& operator<<(Stream&, const SparseMatrix&);
};


Stream& operator<<(Stream&, const SparseMatrix&);


//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit::linalg
